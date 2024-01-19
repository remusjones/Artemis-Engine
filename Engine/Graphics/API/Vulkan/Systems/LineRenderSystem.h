//
// Created by Remus on 19/01/2024.
//

#pragma once
#include "RenderSystemBase.h"


class LineRenderSystem : public RenderSystemBase{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
