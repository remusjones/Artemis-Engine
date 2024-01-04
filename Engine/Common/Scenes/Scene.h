//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <vector>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Objects/FlyCamera.h"
#include "Objects/ImGuiLayer.h"

class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : ImGuiLayer {
public:
    virtual ~Scene() = default;

    virtual void Construct(const char *aSceneName);

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame);

    void OnImGuiRender() override;

    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    void AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline);

    FlyCamera *mActiveSceneCamera;
    // TODO: cleanup these in scene, instead of engine
    std::vector<GraphicsPipeline *> mGraphicsPipelines;
    GPUSceneData mSceneData;
    const char *mSceneName; //
    std::vector<MeshObject *> mObjects;


};
