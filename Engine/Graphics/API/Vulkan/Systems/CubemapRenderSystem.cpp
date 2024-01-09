//
// Created by Remus on 7/01/2024.
//

#include "CubemapRenderSystem.h"

#include "Logger.h"
#include "PipelineConfigInfo.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Cubemap.h"
#include "Base/Common/Buffers/PushConstants.h"

//void CubemapRenderSystem::Create(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts) {
//    mBoundDescriptorLayouts = aDescriptorLayouts;
//    CreatePipelineLayout();
//    CreatePipeline();
//}

void CubemapRenderSystem::CreatePipelineLayout() {
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
        Logger::Log(spdlog::level::critical, "Failed to create Cubemap render pipeline layout");
    }
}

void CubemapRenderSystem::CreatePipeline() {
    assert(mPipelineLayout != nullptr);


    GraphicsPipeline::DefaultPipelineConfigInfo(mPipelineConfig);
    mPipelineConfig.renderPass = gGraphics->mSwapChain->mRenderPass;
    mPipelineConfig.pipelineLayout = mPipelineLayout;
    mPipelineConfig.subpass = static_cast<uint32_t>(GraphicsPipeline::SubPasses::SUBPASS_TRANSPARENCY);
    mPipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;

    mPipelineConfig.depthStencilInfo.depthTestEnable = false;
    mPipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
   // std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
   // attributeDescriptions[0].binding = 0;
   // attributeDescriptions[0].location = 0;
   // attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
   // attributeDescriptions[0].offset = offsetof(glm::vec3, x);
//
   // mPipelineConfig.mAttributeDescriptions = attributeDescriptions;
//
   // VkVertexInputBindingDescription bindingDescription{};
   // bindingDescription.binding = 0;
   // bindingDescription.stride = sizeof(glm::vec3);
   // bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
   // std::vector<VkVertexInputBindingDescription> bindingDescriptions;
   // bindingDescriptions.push_back(bindingDescription);
   // mPipelineConfig.mBindingDescriptions = bindingDescriptions;

    // create a pipeline
    mPipeline = std::make_unique<GraphicsPipeline>("Cubemap Pipeline", mPipelineConfig);
    mPipeline->CreateShaderModule("/Assets/Shaders/Skybox_v.spv", VK_SHADER_STAGE_VERTEX_BIT);
    mPipeline->CreateShaderModule("/Assets/Shaders/Skybox_f.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
}

