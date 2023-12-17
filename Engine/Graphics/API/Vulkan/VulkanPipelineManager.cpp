//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "VulkanPipelineManager.h"
#include "../../../IO/File Management/FileManagement.h"
#include <iostream>
void VulkanPipelineManager::Initialize(VkDevice& aLogicalDevice,
                                       VulkanSwapChain* aSwapChain,
                                       VkPhysicalDevice& aPhysicalDevice,
                                       VkQueue& aGraphicsQueue,
                                       VkQueue& aPresentQueue
                              )
{


    // Register the shader module
    mLogicalDevice = aLogicalDevice;
    mSwapChain = aSwapChain;
    mGraphicsQueue = aGraphicsQueue;
    mPresentQueue = aPresentQueue;
    mPhysicalDevice = aPhysicalDevice;

    // Configuration info population
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProperties);
}

void VulkanPipelineManager::Cleanup()
{
    if (mGraphicsPipeline != nullptr)
    {
        CleanupOldSyncObjects();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(mLogicalDevice, mInFlightFences[i], nullptr);
        }
        if (mCommandPool != nullptr)
        {
            vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);
        }else
        {
            throw std::runtime_error("Attempted to Deconstruct Command Pool, but there was none to deconstruct");
        }


        vkDestroyPipeline(mLogicalDevice, mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(mLogicalDevice, mPipelineLayout, nullptr);
    }else
    {
        throw std::runtime_error("Attempted to Deconstruct Render Pipeline, but there was none to deconstruct");
    }
    vkDestroyRenderPass(mLogicalDevice, mSwapChain->mRenderPass, nullptr);
    // release all loaded shaders

    for(auto & loadedMaterial : mLoadedMaterials)
    {
        VulkanMaterial* targetMaterial = loadedMaterial;

        targetMaterial->CleanupShaderModules(mLogicalDevice);
        delete loadedMaterial;
    }
    CleanupBuffers();
    mLoadedMaterials.resize(0, nullptr);
}

VulkanMaterial* VulkanPipelineManager::LoadShader(const std::string& aShaderName)
{

    std::cout << "Creating Shader: " << aShaderName << std::endl;

    //
    // Attempts to automatically load vert + frag with req suffixes
    //
    std::string vertex = aShaderName;
    std::string frag = aShaderName;

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

    auto fragData = CreateShaderModule(FileManagement::GetShaderFileData(frag));
    auto vertData = CreateShaderModule(FileManagement::GetShaderFileData(vertex));

    //
    // Merge the two into a VulkanMaterial for easy lookup
    //

    VulkanMaterial* material = new VulkanMaterial(fragData, vertData);

    mLoadedMaterials.push_back(material);


    return material;
}

VkShaderModule VulkanPipelineManager::CreateShaderModule(const std::vector<char> &aCode)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = aCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(aCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(mLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;

}

VkResult VulkanPipelineManager::CreateVertexBuffer(const std::vector<Vertex>& aVertices)
{
    mVertexBuffer = new Buffer(aVertices.data(), sizeof(aVertices[0]) * aVertices.size(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    //std::cout << "Creating vertex buffer\n";
    //VkDeviceSize bufferSize = sizeof(aVertices[0]) * aVertices.size();
    //VkBuffer stagingBuffer;
    //VkDeviceMemory stagingBufferMemory;
//
    //std::cout << "\tCreating vertex staging buffer\n";
    //CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
    //void* data;
    //vkMapMemory(mLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
   // memcpy(data, aVertices.data(), (size_t) bufferSize);
    //vkUnmapMemory(mLogicalDevice, stagingBufferMemory);
//
    //std::cout << "\tAllocating vertex buffer\n";
    //CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    //             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);
//
    //CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);
//
    //std::cout <<"\tReleasing Copy Buffers" << std::endl;
    //vkDestroyBuffer(mLogicalDevice, stagingBuffer, nullptr);
    //vkFreeMemory(mLogicalDevice, stagingBufferMemory, nullptr);

    return VK_SUCCESS;
}

VkResult VulkanPipelineManager::CreateIndexBuffer()
{
    mIndexBuffer = new Buffer(indices.data(), sizeof(indices[0]) * indices.size(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    //std::cout << "Creating index buffer\n";
    //VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
//
    //VkBuffer stagingBuffer;
    //VkDeviceMemory stagingBufferMemory;
    //std::cout << "\tCreating index staging buffer\n";
    //CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
    //void* data;
    //vkMapMemory(mLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    //memcpy(data, indices.data(), (size_t) bufferSize);
    //vkUnmapMemory(mLogicalDevice, stagingBufferMemory);
    //std::cout << "\tAllocating index buffer\n";
    //CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    //             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);
//
    //CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);
    //std::cout <<"\tReleasing Copy Buffers" << std::endl;
    //vkDestroyBuffer(mLogicalDevice, stagingBuffer, nullptr);
    //vkFreeMemory(mLogicalDevice, stagingBufferMemory, nullptr);

    return VK_SUCCESS;
}

void VulkanPipelineManager::CreateBuffer(VkDeviceSize aSize, VkBufferUsageFlags aUsage,
                                         VkMemoryPropertyFlags aProperties, VkBuffer& aBuffer, VkDeviceMemory& aBufferMemory)
{


    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = aSize;
    bufferInfo.usage = aUsage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mLogicalDevice, &bufferInfo, nullptr, &aBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create aBuffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(mLogicalDevice, aBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, aProperties);

    if (vkAllocateMemory(mLogicalDevice, &allocInfo, nullptr, &aBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate aBuffer memory!");
    }

    vkBindBufferMemory(mLogicalDevice, aBuffer, aBufferMemory, 0);

}

void VulkanPipelineManager::CopyBuffer(VkBuffer aSrcBuffer, VkBuffer aDstBuffer, VkDeviceSize aSize)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = aSize;
    vkCmdCopyBuffer(commandBuffer, aSrcBuffer, aDstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mGraphicsQueue);

    vkFreeCommandBuffers(mLogicalDevice, mCommandPool, 1, &commandBuffer);

}

void VulkanPipelineManager::CreatePipelineLayout()
{
    std::cout << "Creating Graphics Pipeline Layout\n";

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    // Vertex Input
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();


    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) mSwapChain->mSwapChainExtent.width;
    viewport.height = (float) mSwapChain->mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = mSwapChain->mSwapChainExtent;

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

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    for (auto & m_loadedMaterial : mLoadedMaterials)
    {
        for(const auto & m_shaderStage : m_loadedMaterial->m_shaderStages)
        {
            shaderStages.push_back(m_shaderStage);
        }
    }

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(mLogicalDevice, &pipelineLayoutInfo,
                               nullptr, &mPipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("failed to create pipeline layout");
    }

    std::cout << "\tCreating Graphics Pipeline\n";
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mSwapChain->mRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    if (vkCreateGraphicsPipelines(mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline");
    }

    mSwapChain->CreateFrameBuffers();
}

void VulkanPipelineManager::CreateCommandPool(const QueueFamilyIndices& aQueueFamilyIndices)
{
    std::cout << "Creating Command Pool" << std::endl;
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = aQueueFamilyIndices.mGraphicsFamily.value();

    if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
    CreateCommandBuffers();
}

void VulkanPipelineManager::CreateCommandBuffers()
{
    mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();

    if (vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }
}

bool semaphoresNeedToBeRecreated = false;

void VulkanPipelineManager::DrawFrame()
{
    vkWaitForFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    //
    // Avoid max timout due to weird issue (after swap chain recreation) - https://www.reddit.com/r/vulkan/comments/wfugbz/weird_behaviour_of_vkacquireimagekhr/
    // 14/12/2022
    //
    VkResult result = vkAcquireNextImageKHR(mLogicalDevice,
                                            mSwapChain->mSwapChain,
                                            0,
                                            mImageAvailableSemaphores[mCurrentFrame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
        mSwapChain->RecreateSwapChain();

        CreateCommandBuffers();

        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
        mFramebufferResized = false;
    }else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }


    // Only reset the fence if we are submitting work
    vkResetFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame]);

    vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    // VUID-vkCmdDrawIndexed-None-08608
    RecordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(mLogicalDevice, 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

    VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {mSwapChain->mSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapChain->RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated)
    {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}

void VulkanPipelineManager::CleanupOldSyncObjects()
{
    for (size_t i = 0; i < mRenderFinishedSemaphoresToDestroy.size(); i++) {
        vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphoresToDestroy[i], nullptr);
    }
    mRenderFinishedSemaphoresToDestroy.clear();

    for (size_t i = 0; i < mInFlightFencesToDestroy.size(); i++) {
        vkDestroyFence(mLogicalDevice, mInFlightFencesToDestroy[i], nullptr);
    }
    mInFlightFencesToDestroy.clear();

    for (size_t i = 0; i < mImageAvailableSemaphoresToDestroy.size(); i++) {
        vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphoresToDestroy[i], nullptr);
    }
    mImageAvailableSemaphoresToDestroy.clear();
}

void VulkanPipelineManager::CreateSyncObjects()
{
    mInFlightFencesToDestroy.insert(mInFlightFencesToDestroy.end(), std::begin(mInFlightFences), std::end(mInFlightFences));     // C++11
    mImageAvailableSemaphoresToDestroy.insert(mImageAvailableSemaphoresToDestroy.end(), std::begin(mImageAvailableSemaphores), std::end(mImageAvailableSemaphores));     // C++11
    mRenderFinishedSemaphoresToDestroy.insert(mRenderFinishedSemaphoresToDestroy.end(), std::begin(mRenderFinishedSemaphores), std::end(mRenderFinishedSemaphores));     // C++11


    std::cout << "Creating Semaphores and Fences" << std::endl;
    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    mImagesInFlight.resize(mSwapChain->mSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(mLogicalDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

uint32_t VulkanPipelineManager::FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((aTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanPipelineManager::CleanupBuffers() {

    mVertexBuffer->Cleanup();
    mIndexBuffer->Cleanup();

    delete mVertexBuffer;
    delete mIndexBuffer;
}

void VulkanPipelineManager::RecordCommandBuffer(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(aCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mSwapChain->mRenderPass;
    renderPassInfo.framebuffer =  mSwapChain->mSwapChainFrameBuffers[aImageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChain->mSwapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(aCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) mSwapChain->mSwapChainExtent.width;
    viewport.height = (float) mSwapChain->mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(aCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = mSwapChain->mSwapChainExtent;
    vkCmdSetScissor(aCommandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {mVertexBuffer->mBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(aCommandBuffer, mIndexBuffer->mBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(aCommandBuffer);

    if (vkEndCommandBuffer(aCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}




