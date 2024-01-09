//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"


class Cubemap;

class LightingRenderSystem {
public:
    LightingRenderSystem(std::vector<VkDescriptorSetLayout> aDescriptorLayouts);

private:
    void CreatePipelineLayout();
    void CreatePipeline();

    VkPipelineLayout mPipelineLayout;
    std::vector<VkDescriptorSetLayout> mBoundDescriptorLayouts;
    PipelineConfigInfo mPipelineConfig;

public:
    std::unique_ptr<GraphicsPipeline> mPipeline;
};
