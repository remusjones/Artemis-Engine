//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vector>

#include "..\..\Base\Common\Buffers\PushConstants.h"
#include "Base/Common/Data/Mesh.h"
#include "Base/Common/Data/Vertex.h"
#include "Math/Transform.h"
#include "Objects/Super.h"

class Mesh;
class Material;
class VulkanEngine;
class GraphicsPipeline;
class Buffer;
class CameraInformationBuffer;
class AllocatedVertexBuffer;

class Renderer {
public:
    Renderer() = default;

    virtual ~Renderer() = default;

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame, glm::mat4 aProjectionMatrix);
    void LoadMesh(const char* aPath) const { mMesh->LoadFromObject(aPath); }
    virtual void CreateRenderer(GraphicsPipeline &aBoundGraphicsPipeline) = 0;
    virtual void DestroyRenderer() = 0;
    GraphicsPipeline *mGraphicsPipeline;

    Mesh* mMesh;
    Material* mMaterial;
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


    void CreateRenderer(GraphicsPipeline &aBoundGraphicsPipeline) override;
    void DestroyRenderer() override;
    void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                uint32_t aCurrentFrame, glm::mat4 aCameraViewMatrix) override;

    // TODO: Move name to a metadata tag instead
    const char *mName;

    Transform mTransform;
};
