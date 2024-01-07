//
// Created by Remus on 7/01/2024.
//

#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"


class Cubemap;

class CubemapRenderSystem {
public:
    CubemapRenderSystem(std::vector<VkDescriptorSetLayout> aDescriptorLayouts);

private:
    void CreatePipelineLayout();

    void CreatePipeline();

    VkPipelineLayout mPipelineLayout;
    std::vector<VkDescriptorSetLayout> mBoundDescriptorLayouts;

    PipelineConfigInfo mPipelineConfig{};

public:
    std::unique_ptr<GraphicsPipeline> mPipeline;
};
