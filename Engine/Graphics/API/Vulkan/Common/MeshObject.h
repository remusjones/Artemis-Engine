//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vector>

#include "..\..\Base\Common\Buffers\PushConstants.h"
#include "Base/Common/Data/Vertex.h"
#include "Math/Transform.h"
#include "Objects/Super.h"

class Mesh;
class Material;
class VulkanEngine;
class GraphicsPipeline;
class Buffer;
class UniformBuffer;
class AllocatedVertexBuffer;

class Renderer {
public:
    Renderer();

    virtual ~Renderer();

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame, glm::mat4 aProjectionMatrix);

    virtual void InitializeRenderer(GraphicsPipeline &aBoundGraphicsPipeline) =
    0;
    GraphicsPipeline *mGraphicsPipeline;

    Mesh* mMesh;
    PushConstants mPushConstants{};
};

/*Attempts to abstract the required components for rendering to
 * identify what can be seperated from render pipeline */
class MeshObject : public Renderer, Super {
public:

    void Construct() override;
    void Tick(float aDeltaTime) override;
    void Cleanup() override;

    void CreateObject(GraphicsPipeline &aBoundGraphicsPipeline,
                      const char *aName = "Default");


    void InitializeRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;

    void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                uint32_t aCurrentFrame, glm::mat4 aCameraViewMatrix) override;

    // TODO: Move name to a metadata tag instead
    const char *mName;

    Transform mTransform;
};
