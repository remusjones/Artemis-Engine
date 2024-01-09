//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Renderer.h"


class PrimativeMesh;
class Mesh;

class SkyboxRenderer final : public Renderer {
public:
    void Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) override;
    void BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;
    void DestroyRenderer() override;

    void LoadMesh(const char *aPath, const char *aMtlPath = "");

    Transform *mTransform;
    PrimativeMesh *mMesh;
};
