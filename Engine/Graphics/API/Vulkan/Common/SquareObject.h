//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vector>
#include "API/Base/Common/Data/Vertex.h"

class Material;
class VulkanPipelineManager;
class GraphicsPipeline;
class Buffer;

class Renderer{
public:
    virtual void Render(VkCommandBuffer aCommandBuffer) = 0;
};

/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class SquareObject : public Renderer {
public:
    void CreateObject(GraphicsPipeline& aBoundGraphicsPipeline);
    void Render(VkCommandBuffer aCommandBuffer) override;
    void Destroy();
    // Rendering Data
    Material* mMaterial;
    GraphicsPipeline* mGraphicsPipeline;
    // Mesh Data
    std::vector<Vertex> mVertices;
    std::vector<int16_t> mIndices;
    Buffer* mVertexBuffer;
    Buffer* mIndexBuffer;
};
