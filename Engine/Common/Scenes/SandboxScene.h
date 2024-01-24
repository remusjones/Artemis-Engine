//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Common/MaterialFactory.h"
#include "Vulkan/Systems/PBRRenderSystem.h"
#include "Vulkan/Systems/PipelineConfigInfo.h"
#include "Vulkan/Systems/SkyboxRenderSystem.h"
#include "Vulkan/Systems/UnlitRenderSystem.h"
#include "Vulkan/Systems/WireframeRenderSystem.h"

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
    std::unordered_map<std::string, Texture *> mLoadedTextures;
    PipelineConfigInfo mDefaultPipelineConfig;

    VkSampler mBlockySampler;
    MeshObject *mMonkey;
    MeshObject *mTeapot;
    MeshObject *mLight;
    MeshObject *mFloor;
    Primative *mCubemapMesh;

    std::shared_ptr<Cubemap> mCubemap;
    std::unique_ptr<SkyboxRenderSystem> mCubemapPipeline;
    std::shared_ptr<UnlitRenderSystem> mUnlitPipeline;
    std::shared_ptr<WireframeRenderSystem> mWireframeRenderSystem;
    std::shared_ptr<PBRRenderSystem> mPBRPipeline;

    MaterialFactory mMaterialUnlitFactory;
    MaterialFactory mMaterialPBRFactory;
    MaterialFactory mGenericMaterialFactory;
};
