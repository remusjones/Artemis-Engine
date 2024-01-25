//
// Created by Remus on 24/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Data/Vertex.h"

/*
 * Draws line(s) between "positions"
 */
class LineRenderer : public Renderer {
public:
    void SetLinePositions(const std::vector<glm::vec3>& aPositions);

    void DestroyRenderer() override;

    void BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;

    void Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) override;

    std::vector<Position> mLinePositions;
    AllocatedBuffer *mAllocatedPositions;
    Transform *mTransform;
};
