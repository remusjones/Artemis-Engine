//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"


class RenderSystemBase {
public:
    virtual void Create(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts);

protected:
    virtual void CreatePipelineLayout() = 0;
    virtual void CreatePipeline() = 0;
    void CreatePipelineObject(const char* pipelineName, bool aDestructive = false);


public:
    std::unique_ptr<GraphicsPipeline> mPipeline;
    std::vector<VkDescriptorSetLayout> GetBoundDescriptors() {return mBoundDescriptorLayouts;}
protected:
    VkPipelineLayout mPipelineLayout = nullptr;
    std::vector<VkDescriptorSetLayout> mBoundDescriptorLayouts;
    PipelineConfigInfo mPipelineConfig{};
};
