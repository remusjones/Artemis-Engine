//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <vector>

#include "Base/Common/Buffers/AllocatedBuffer.h"

class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Construct(const char *aSceneName);

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame);

    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    void AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline);

    Camera *mActiveCamera;
    std::vector<MeshObject *> mObjects;
    // TODO: cleanup these in scene, instead of engine
    std::vector<GraphicsPipeline *> mGraphicsPipelines;
    const char *mSceneName;
};
