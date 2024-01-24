//
// Created by Remus on 25/01/2024.
//

#pragma once
#include "RenderSystemBase.h"


class WireframeRenderSystem : public RenderSystemBase{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
