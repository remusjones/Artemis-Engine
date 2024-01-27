//
// Created by Remus on 24/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Data/Vertex.h"

class Color;
/*
 * Draws line(s) between "positions"
 */
class LineRenderer : public Renderer {
public:
    enum LineRenderMode {
        // Draws a continuous connected line
        LINES_CONTINUOUS,
        // Draws a line between each position
        LINES_SEGMENTED,
    };

    void SetLinePositions(const std::vector<glm::vec3> &aPositions, const LineRenderMode aMode = LINES_CONTINUOUS);

    void SetLinePositions(const std::vector<glm::vec3> &aPositions, const std::vector<Color> &aColors,
        const LineRenderMode aMode = LINES_CONTINUOUS);

    void DestroyRenderer() override;

    void BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;

    void Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) override;

    std::vector<Vertex> mLinePositions;
    AllocatedBuffer *mAllocatedPositions;
    Transform *mTransform;

private:
    LineRenderMode mLineRenderMode{};
};
