//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "RenderSystemBase.h"


class PBRRenderSystem : public RenderSystemBase{
    void CreatePipelineLayout() override;
    void CreatePipeline() override;
};
