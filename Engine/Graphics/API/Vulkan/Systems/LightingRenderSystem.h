//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GraphicsPipeline.h"
#include "RenderSystemBase.h"


class Cubemap;

class LightingRenderSystem : public RenderSystemBase {
public:
    LightingRenderSystem(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts);
protected:
    void CreatePipelineLayout() override;
    void CreatePipeline() override;

};
