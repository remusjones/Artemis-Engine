//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "RemPipeline.h"
#include <iostream>
#include <Helpers/File Management/RemFileManagement.h>
void RemPipeline::Initialize(VkDevice& logicalDevice,
                             RemSwapChain* remSwapChain,
                              VkPhysicalDevice& physicalDevice,
                              VkQueue& graphicsQueue,
                              VkQueue& presentQueue
                              )
{


    // Register the shader module
    m_logicalDevice = logicalDevice;
    m_remSwapChain = remSwapChain;
    m_graphicsQueue = graphicsQueue;
    m_presentQueue = presentQueue;



    //
    // Configuration info population
    //
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
}

void RemPipeline::Cleanup()
{
    if (m_hasCreatedPipeline)
    {
        CleanupOldSyncObjects();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_logicalDevice, m_inFlightFences[i], nullptr);
        }
        if (m_commandPool != nullptr)
        {
            vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr);
        }else
        {
            throw std::runtime_error("Attempted to Deconstruct Command Pool, but there was none to deconstruct");
        }


        vkDestroyPipeline(m_logicalDevice, m_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr);
    }else
    {
        throw std::runtime_error("Attempted to Deconstruct Render Pipeline, but there was none to deconstruct");
    }
    vkDestroyRenderPass(m_logicalDevice, m_remSwapChain->m_renderPass, nullptr);
    // release all loaded shaders

    for(int i = 0; i < m_loadedShaders.size(); i++)
    {
        if (m_loadedShaders[i]->m_fragment != nullptr)
            vkDestroyShaderModule(m_logicalDevice, m_loadedShaders[i]->m_fragment, nullptr);
        if (m_loadedShaders[i]->m_vertex != nullptr)
            vkDestroyShaderModule(m_logicalDevice, m_loadedShaders[i]->m_vertex, nullptr);

        delete m_loadedShaders[i];
    }

    m_loadedShaders.resize(0,nullptr);
}

VkResult RemPipeline::LoadShader(const std::string& shaderName)
{

    std::cout << "Creating Shader: " << shaderName << std::endl;


    //
    // Attempts to automatically load vert + frag with req suffixes
    //
    std::string vertex = shaderName;
    std::string frag = shaderName;

    FileManagementShaderInfo fileSettings;

    //
    // Append required suffixes to locate file
    //
    vertex.append(fileSettings.pShaderVertexSuffix);
    vertex.append(fileSettings.pShaderFileExtension);

    frag.append(fileSettings.pShaderFragmentSuffix);
    frag.append(fileSettings.pShaderFileExtension);



    //
    // Load and create module
    //
    auto fragData = CreateShaderModule(RemFileManagement::GetShaderFileData(frag));
    auto vertData = CreateShaderModule(RemFileManagement::GetShaderFileData(vertex));

    //
    // Merge the two into a RemShaderComponent for easy lookup
    //
    m_loadedShaders.push_back(new RemShaderComponent(fragData, vertData));

    return VK_SUCCESS;
}


VkShaderModule RemPipeline::CreateShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;

}

//TODO: Move this into Shader->Setup

void RemPipeline::CreatePipelineLayout()
{
    std::cout << "Creating Graphics Pipeline Layout" << std::endl;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) m_remSwapChain->m_swapChainExtent.width;
    viewport.height = (float) m_remSwapChain->m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_remSwapChain->m_swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(m_logicalDevice, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    std::cout << "Creating Graphics Pipeline" << std::endl;
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;


    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    for (int i = 0; i < m_loadedShaders.size(); i++)
    {
        for(int j = 0; j < m_loadedShaders[i]->m_shaderStages.size(); j++)
        {
            shaderStages.push_back(m_loadedShaders[i]->m_shaderStages[j]);
        }
    }

    pipelineInfo.pStages = shaderStages.data();

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional

    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_remSwapChain->m_renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(m_logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    m_remSwapChain->CreateFrameBuffers();

    m_hasCreatedPipeline = true;
}

void RemPipeline::CreateCommandPool(QueueFamilyIndices& queueFamilyIndices)
{
    std::cout << "Creating Command Pool" << std::endl;

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.m_graphicsFamily.value();
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    CreateCommandBuffer();
    CreateSyncObjects();

}

void RemPipeline::CreateCommandBuffer()
{
    std::cout << "Creating Command Buffer" << std::endl;
    m_commandBuffers.resize(m_remSwapChain->m_swapChainFrameBuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) m_commandBuffers.size();

    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    for (size_t i = 0; i < m_commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_remSwapChain->m_renderPass;
        renderPassInfo.framebuffer = m_remSwapChain->m_swapChainFrameBuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_remSwapChain->m_swapChainExtent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
        vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(m_commandBuffers[i]);
        if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

    }

}

bool semaphoresNeedToBeRecreated = false;
void RemPipeline::DrawFrame()
{
    vkWaitForFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    //
    // Avoid max timout due to weird issue (after swap chain recreation) - https://www.reddit.com/r/vulkan/comments/wfugbz/weird_behaviour_of_vkacquireimagekhr/
    // 14/12/2022
    //
    VkResult result = vkAcquireNextImageKHR(m_logicalDevice,
                                            m_remSwapChain->m_swapChain,
                                            0,
                                            m_imageAvailableSemaphores[m_currentFrame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
        m_remSwapChain->RecreateSwapChain();

        //
        // Note causes an validation issue when recreating the command buffer, causing frames in flight to become invalid .. ?
        //
        CreateCommandBuffer();

        //
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        //
        semaphoresNeedToBeRecreated = true;
        m_framebufferResized = false;
    }else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }


    // Only reset the fence if we are submitting work
    vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(m_logicalDevice, 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    m_imagesInFlight[imageIndex] = m_inFlightFences[m_currentFrame];
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];


    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_logicalDevice, 1, &m_inFlightFences[m_currentFrame]);
    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_remSwapChain->m_swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        m_remSwapChain->RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated)
    {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}
void RemPipeline::CleanupOldSyncObjects()
{
    for (size_t i = 0; i < m_renderFinishedSemaphoresToDestroy.size(); i++) {
        vkDestroySemaphore(m_logicalDevice, m_renderFinishedSemaphoresToDestroy[i], nullptr);
    }
    m_renderFinishedSemaphoresToDestroy.clear();

    for (size_t i = 0; i < m_inFlightFencesToDestroy.size(); i++) {
        vkDestroyFence(m_logicalDevice, m_inFlightFencesToDestroy[i], nullptr);
    }
    m_inFlightFencesToDestroy.clear();

    for (size_t i = 0; i < m_imageAvailableSemaphoresToDestroy.size(); i++) {
        vkDestroySemaphore(m_logicalDevice, m_imageAvailableSemaphoresToDestroy[i], nullptr);
    }
    m_imageAvailableSemaphoresToDestroy.clear();
}
void RemPipeline::CreateSyncObjects()
{

    //
    // Add out of date fences for destruction when gpu free releases frame
    //
    m_inFlightFencesToDestroy.insert(m_inFlightFencesToDestroy.end(), std::begin(m_inFlightFences), std::end(m_inFlightFences));     // C++11
    m_imageAvailableSemaphoresToDestroy.insert(m_imageAvailableSemaphoresToDestroy.end(), std::begin(m_imageAvailableSemaphores), std::end(m_imageAvailableSemaphores));     // C++11
    m_renderFinishedSemaphoresToDestroy.insert(m_renderFinishedSemaphoresToDestroy.end(), std::begin(m_renderFinishedSemaphores), std::end(m_renderFinishedSemaphores));     // C++11


    std::cout << "Creating Semaphores and Fences" << std::endl;
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_imagesInFlight.resize(m_remSwapChain->m_swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void RemPipeline::DestroyShader(RemShaderComponent* shaderComponent)
{
    //if ( std::find(m_loadedShaders.begin(), m_loadedShaders.end(), shaderComponent) != m_loadedShaders.end() )
    //{
    //    std::remove(m_loadedShaders.begin(), m_loadedShaders.end(), shaderComponent);
//
    //    if (shaderComponent->m_fragment != nullptr)
    //        vkDestroyShaderModule(m_logicalDevice, shaderComponent->m_fragment, nullptr);
    //    if (shaderComponent->m_vertex != nullptr)
    //        vkDestroyShaderModule(m_logicalDevice, shaderComponent->m_vertex, nullptr);
//
    //    delete shaderComponent;
    //    m_loadedShaders.shrink_to_fit();
//
    //}else
    //{
    //    throw std::runtime_error("Shader not registered");
    //}
}



