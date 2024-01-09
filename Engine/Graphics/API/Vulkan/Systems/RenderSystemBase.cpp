//
// Created by Remus on 9/01/2024.
//

#include "RenderSystemBase.h"

#include "Logger.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Buffers/PushConstants.h"

void RenderSystemBase::Create(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts) {
    mBoundDescriptorLayouts = aDescriptorLayouts;
    CreatePipelineLayout();
    CreatePipeline();
}

void RenderSystemBase::CreatePipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(mBoundDescriptorLayouts.size());
    pipelineLayoutInfo.pSetLayouts = mBoundDescriptorLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(gGraphics->mLogicalDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) !=
        VK_SUCCESS) {
        Logger::Log(spdlog::level::critical, "Failed to create <RenderSystemBase> render pipeline layout");
    }
}

void RenderSystemBase::CreatePipeline() {
    assert(mPipelineLayout != nullptr);

    Logger::Log(spdlog::level::warn,
        "Creating <RenderSystemBase> render pipeline, consider creating an implementation instead");

    GraphicsPipeline::DefaultPipelineConfigInfo(mPipelineConfig);
    mPipelineConfig.renderPass = gGraphics->mSwapChain->mRenderPass;
    mPipelineConfig.pipelineLayout = mPipelineLayout;
    mPipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_GEOMETRY);
    mPipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;

    mPipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
    mPipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

    // create a pipeline
    mPipeline = std::make_unique<GraphicsPipeline>("RenderSystemBase", mPipelineConfig);
    mPipeline->CreateShaderModule("/Assets/Shaders/3DObject_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    mPipeline->CreateShaderModule("/Assets/Shaders/TexturedLit_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}
