//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Base/Common/Buffers/PushConstants.h"
#include "Scenes/Scene.h"

class Material;

class Renderer {
public:
    Renderer() = default;

    virtual ~Renderer() = default;
    virtual void Render(VkCommandBuffer aCommandBuffer, const Scene &aScene);
    virtual void BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline);
    virtual void DestroyRenderer();

    GraphicsPipeline *mGraphicsPipeline;
    Material *mMaterial;
    PushConstants mPushConstants{};
};
