//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Common/MaterialFactory.h"
#include "Vulkan/Systems/LineRenderSystem.h"
#include "Vulkan/Systems/PBRRenderSystem.h"
#include "Vulkan/Systems/PipelineConfigInfo.h"
#include "Vulkan/Systems/SkyboxRenderSystem.h"
#include "Vulkan/Systems/UnlitRenderSystem.h"
#include "Vulkan/Systems/WireframeRenderSystem.h"

class LineRenderer;
class Cubemap;
class Primative;
class Texture;

class SandboxScene final : public Scene {
public:
    void PreConstruct(const char *aSceneName) override;

    void Construct() override;

    void Tick(float aDeltaTime) override;

    void Cleanup() override;

    void OnImGuiRender() override;

private:
    PipelineConfigInfo mDefaultPipelineConfig;

    VkSampler mBlockySampler;

    // TODO: Remove unmanaged pointer usage
    MeshObject *mMonkey;
    MeshObject *mTeapot;
    MeshObject *mLight;
    MeshObject *mFloor;
    LineRenderer* mLineRenderer;
    Primative *mCubemapMesh;
    Primative *mLineRendererEntity;

    Cubemap* mCubemap;
    std::unique_ptr<SkyboxRenderSystem> mCubemapPipeline;
    std::unique_ptr<UnlitRenderSystem> mUnlitPipeline;
    std::unique_ptr<WireframeRenderSystem> mWireframeRenderSystem;
    std::unique_ptr<PBRRenderSystem> mPBRPipeline;
    std::unique_ptr<LineRenderSystem> mLineRenderPipeline;

    MaterialFactory mMaterialUnlitFactory;
    MaterialFactory mMaterialPBRFactory;
    MaterialFactory mGenericMaterialFactory;
};
