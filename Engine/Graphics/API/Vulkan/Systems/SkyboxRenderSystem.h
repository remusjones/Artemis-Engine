//
// Created by Remus on 7/01/2024.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "RenderSystemBase.h"


class Cubemap;

class SkyboxRenderSystem : public RenderSystemBase {
protected:
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};