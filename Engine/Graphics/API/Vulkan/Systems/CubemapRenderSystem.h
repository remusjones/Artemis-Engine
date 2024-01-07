//
// Created by Remus on 7/01/2024.
//

#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

class GraphicsPipeline;

class CubemapRenderSystem {
public:
    CubemapRenderSystem(VkRenderPass aRenderPass, std::vector<VkDescriptorSetLayout>& aDescriptorSetLayouts);
private:
    void CreatePipelineLayout(std::vector<VkDescriptorSetLayout>& aDescriptorSetLayouts);
    void CreatePipeline(VkRenderPass aRenderPass);

    VkPipelineLayout mPipelineLayout;
    std::unique_ptr<GraphicsPipeline> mPipeline;
};
