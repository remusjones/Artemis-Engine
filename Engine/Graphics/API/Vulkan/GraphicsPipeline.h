//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vector>

class GraphicsPipeline {
public:
    void AddShader(const char* aPath, VkShaderStageFlagBits aStage);
    void Create();
    void Destroy() const;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
};
