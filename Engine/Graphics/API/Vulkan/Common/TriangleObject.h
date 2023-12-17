//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vector>
#include "API/Base/Common/Data/Vertex.h"

class Material;
class VulkanPipelineManager;
class GraphicsPipeline;
/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class TriangleObject {
public:
    void CreateObject();
    void Render(VkCommandBuffer aCommandBuffer);
    void Destroy();
    // Rendering Data
    Material* mMaterial;
    GraphicsPipeline* mGraphicsPipeline;
    // Mesh Data
    std::vector<Vertex> mVertices;
    std::vector<int16_t> mIndices;
};
