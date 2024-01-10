//
// Created by Remus on 10/01/2024.
//

#pragma once
#include "RenderSystemBase.h"

class IrradianceRenderSystem : RenderSystemBase {
protected:
    void CreatePipelineLayout() override;

    void CreatePipeline() override;
};
