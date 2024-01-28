//
// Created by Remus on 17/12/2023.
//

#include <vulkan/vulkan_core.h>
#include "GraphicsPipeline.h"

#include <stdexcept>

#include "Logger.h"
#include "PipelineConfigInfo.h"
#include "Base/Common/Data/Vertex.h"
#include "VulkanGraphicsImpl.h"
#include "File Management/FileManagement.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "Base/Common/Material.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Scenes/Scene.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void GraphicsPipeline::Create() {
    Logger::Log(spdlog::level::info,
                (std::string("Creating Graphics Pipeline ") + mPipelineName).c_str());
    if (mPipelineConfig.pipelineLayout == VK_NULL_HANDLE) {
        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(PushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

        // Bind these elsewhere?
        std::vector<VkDescriptorSetLayout> layouts;
        for (int i = 0; i < mRenderers.size(); i++) {
            layouts.push_back(mRenderers[i]->mMaterial->GetDescriptorLayout());
        }
        //hook the global set layout
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();


        if (vkCreatePipelineLayout(gGraphics->mLogicalDevice, &pipelineLayoutInfo,
                                   nullptr, &mPipelineConfig.pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }


        mPipelineConfig.depthStencilInfo = VulkanInitialization::DepthStencilCreateInfo(true, true,
            VK_COMPARE_OP_LESS_OR_EQUAL);
        mPipelineConfig.renderPass = gGraphics->mSwapChain->mRenderPass;
    }

    // Vertex Information
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(
        mPipelineConfig.mAttributeDescriptions.size());

    vertexInputInfo.pVertexBindingDescriptions = mPipelineConfig.mBindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = mPipelineConfig.mAttributeDescriptions.data();

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pDepthStencilState = &mPipelineConfig.depthStencilInfo;
    pipelineInfo.pStages = mShadersInPipeline.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &mPipelineConfig.inputAssemblyInfo;
    pipelineInfo.pViewportState = &mPipelineConfig.viewportInfo;
    pipelineInfo.pRasterizationState = &mPipelineConfig.rasterizationInfo;
    pipelineInfo.pMultisampleState = &mPipelineConfig.multisampleInfo;
    pipelineInfo.pColorBlendState = &mPipelineConfig.colorBlendInfo;
    pipelineInfo.pDynamicState = &mPipelineConfig.dynamicStateInfo;
    pipelineInfo.layout = mPipelineConfig.pipelineLayout;
    pipelineInfo.renderPass = mPipelineConfig.renderPass;
    // TODO: Increase amount of subpasses
    pipelineInfo.subpass = 0; // mPipelineConfig.subpass;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(gGraphics->mLogicalDevice, VK_NULL_HANDLE, 1,
                                  &pipelineInfo, nullptr,
                                  &mGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline");
    }
}

void GraphicsPipeline::CreateShaderModule(const char *aPath,
                                          const VkShaderStageFlagBits aStage) {
    const constexpr char *entryName = "main";

    const std::vector<char> file = FileManagement::GetShaderFileDataPath(aPath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(file.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(gGraphics->mLogicalDevice, &createInfo, nullptr,
                             &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module");
    }

    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = aStage;
    shaderStageInfo.module = shaderModule;
    shaderStageInfo.pName = entryName;

    mShadersInPipeline.push_back(shaderStageInfo);
}

void GraphicsPipeline::Destroy() {
    Logger::Log(spdlog::level::info, (std::string("Graphics Destroying Pipeline ") + mPipelineName).c_str());
    for (const auto &i: mShadersInPipeline) {
        vkDestroyShaderModule(gGraphics->mLogicalDevice, i.module, nullptr);
    }

    vkDestroyPipelineLayout(gGraphics->mLogicalDevice, mPipelineConfig.pipelineLayout,
                            nullptr);
    vkDestroyPipeline(gGraphics->mLogicalDevice, mGraphicsPipeline, nullptr);
    mGraphicsPipeline = VK_NULL_HANDLE;
    mPipelineConfig.pipelineLayout = VK_NULL_HANDLE;
    mShadersInPipeline.resize(0);
}


void GraphicsPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo &aConfigInfo) {
    aConfigInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    aConfigInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    aConfigInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    aConfigInfo.inputAssemblyInfo.pNext = nullptr;
    aConfigInfo.inputAssemblyInfo.flags = 0;

    aConfigInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    aConfigInfo.viewportInfo.viewportCount = 1;
    aConfigInfo.viewportInfo.pViewports = nullptr;
    aConfigInfo.viewportInfo.scissorCount = 1;
    aConfigInfo.viewportInfo.pScissors = nullptr;

    aConfigInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    aConfigInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    aConfigInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    aConfigInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    aConfigInfo.rasterizationInfo.lineWidth = 1.0f;
    aConfigInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    aConfigInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    aConfigInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    aConfigInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
    aConfigInfo.rasterizationInfo.depthBiasClamp = 0.0f; // Optional
    aConfigInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional

    aConfigInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    aConfigInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    aConfigInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    aConfigInfo.multisampleInfo.minSampleShading = 1.0f; // Optional
    aConfigInfo.multisampleInfo.pSampleMask = nullptr; // Optional
    aConfigInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    aConfigInfo.multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional


    aConfigInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    aConfigInfo.colorBlendAttachment.blendEnable = VK_TRUE;
    aConfigInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    aConfigInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    aConfigInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    aConfigInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    aConfigInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    aConfigInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    aConfigInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    aConfigInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    aConfigInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    aConfigInfo.colorBlendInfo.attachmentCount = 1;
    aConfigInfo.colorBlendInfo.pAttachments = &aConfigInfo.colorBlendAttachment;
    aConfigInfo.colorBlendInfo.blendConstants[0] = 0.0f; // Optional
    aConfigInfo.colorBlendInfo.blendConstants[1] = 0.0f; // Optional
    aConfigInfo.colorBlendInfo.blendConstants[2] = 0.0f; // Optional
    aConfigInfo.colorBlendInfo.blendConstants[3] = 0.0f; // Optional

    aConfigInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    aConfigInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    aConfigInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    aConfigInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    aConfigInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    aConfigInfo.depthStencilInfo.minDepthBounds = 0.0f; // Optional
    aConfigInfo.depthStencilInfo.maxDepthBounds = 1.0f; // Optional
    aConfigInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    aConfigInfo.depthStencilInfo.front = {}; // Optional
    aConfigInfo.depthStencilInfo.back = {}; // Optional

    aConfigInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    aConfigInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    aConfigInfo.dynamicStateInfo.pDynamicStates = aConfigInfo.dynamicStateEnables.data();
    aConfigInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(aConfigInfo.dynamicStateEnables.size());
    aConfigInfo.dynamicStateInfo.flags = 0;
    aConfigInfo.dynamicStateInfo.pNext = VK_NULL_HANDLE;

    aConfigInfo.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_GEOMETRY);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0] = Vertex::GetBindingDescription();

    aConfigInfo.mBindingDescriptions = bindingDescriptions;
    aConfigInfo.mAttributeDescriptions = Vertex::GetAttributeDescriptions();
}

void GraphicsPipeline::AddRenderer(Renderer *aRenderer) {
    mRenderers.push_back(aRenderer);
}

void GraphicsPipeline::Draw(VkCommandBuffer aCommandBuffer, const Scene &aScene) const {
    if (mGraphicsPipeline == nullptr)
        return;

    vkCmdBindPipeline(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      mGraphicsPipeline);

    const FrameData currentFrame = gGraphics->mVulkanEngine.GetCurrentFrame();

    AllocatedBuffer::MapMemory(
    gGraphics->mAllocator, &aScene.mSceneData,  currentFrame.mSceneBuffer.GetAllocation(),
    sizeof(GPUSceneData));

    for (auto &mRenderer: mRenderers) {
        std::vector<VkDescriptorSet> descriptorSets;
        descriptorSets.push_back(mRenderer->mMaterial->GetDescriptorSet());

        vkCmdBindDescriptorSets(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineConfig.pipelineLayout, 0,
                                descriptorSets.size(), descriptorSets.data(), 0, nullptr);

        mRenderer->Render(aCommandBuffer, aScene);
    }
}
