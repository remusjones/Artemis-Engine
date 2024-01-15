//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Common/MaterialFactory.h"
#include "..\..\Graphics\API\Vulkan\Systems\SkyboxRenderSystem.h"
#include "Vulkan/Systems/PBRRenderSystem.h"
#include "Vulkan/Systems/RenderSystemBase.h"
#include "Vulkan/Systems/UnlitRenderSystem.h"

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
    std::shared_ptr<PBRRenderSystem> mPBRPipeline;
    std::shared_ptr<UnlitRenderSystem> mUnlitPipeline;

    MaterialFactory mMaterialUnlitFactory;
    MaterialFactory mMaterialPBRFactory;
    MaterialFactory mGenericMaterialFactory;
};
