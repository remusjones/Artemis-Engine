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
class UniformBuffer;

class Renderer{
public:
    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex, uint32_t aCurrentFrame) = 0;
    // Buffers
    Buffer* mVertexBuffer;
    Buffer* mIndexBuffer;
    UniformBuffer* mUniformBuffer;

    // Rendering Data
    Material* mMaterial; // TODO: Concept Unused
    GraphicsPipeline* mGraphicsPipeline;

    // Mesh Data
    std::vector<Vertex> mVertices;
    std::vector<int16_t> mIndices;
};

/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class SquareObject : public Renderer {
public:
    void CreateObject(GraphicsPipeline& aBoundGraphicsPipeline);
    void RotateObject(uint32_t aCurrentFrame);
    void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex, uint32_t aCurrentFrame) override;
    void Destroy();
};
