//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Common/MaterialFactory.h"
#include "Vulkan/Systems/CubemapRenderSystem.h"
#include "Vulkan/Systems/PBRRenderSystem.h"
#include "Vulkan/Systems/RenderSystemBase.h"
#include "Vulkan/Systems/UnlitRenderSystem.h"

class Primative;
class Texture;

class SandboxScene final : public Scene {
public:
    void Construct(const char *aSceneName) override;

    void Tick(float aDeltaTime) override;

    void Cleanup() override;

private:
    std::unordered_map<std::string, Texture *> mLoadedTextures;
    PipelineConfigInfo mDefaultPipelineConfig;

    VkSampler mBlockySampler;
    MeshObject *mMonkey;
    MeshObject *mTeapot;
    MeshObject *mLight;
    MeshObject *mSphere;
    MeshObject *mCube;
    Primative *mCubemapMesh;

    std::shared_ptr<Cubemap> mCubemap;
    std::unique_ptr<CubemapRenderSystem> mCubemapPipeline;
    std::shared_ptr<PBRRenderSystem> mPBRPipeline;
    std::shared_ptr<UnlitRenderSystem> mUnlitPipeline;

    MaterialFactory mMaterialUnlitFactory;
    MaterialFactory mMaterialPBRFactory;
    MaterialFactory mGenericMaterialFactory;
};
