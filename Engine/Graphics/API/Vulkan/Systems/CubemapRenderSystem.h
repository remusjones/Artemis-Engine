//
// Created by Remus on 7/01/2024.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "RenderSystemBase.h"


class Cubemap;

class CubemapRenderSystem : public RenderSystemBase {
public:
 //   void Create(const std::vector<VkDescriptorSetLayout> &aDescriptorLayouts) override;

protected:
    void CreatePipelineLayout() override;

    void CreatePipeline() override;

};
