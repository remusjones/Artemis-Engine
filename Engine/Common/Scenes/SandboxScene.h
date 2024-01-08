//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>
#include "Scene.h"
#include "Vulkan/Systems/CubemapRenderSystem.h"

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
    MeshObject *mMonkey2;
    MeshObject *mTeapot;
    MeshObject *mLight;
    MeshObject *mSphere;
    MeshObject *mCube;
    MeshObject *mCubemapMesh;

    std::shared_ptr<Cubemap> mCubemap;
    std::unique_ptr<CubemapRenderSystem> mCubemapPipeline;
    std::unique_ptr<GraphicsPipeline> mTexturedMeshPipeline;
    std::unique_ptr<GraphicsPipeline> mHalfLambertMeshPipeline;
    std::unique_ptr<GraphicsPipeline> mUnlitMeshPipeline;
    std::unique_ptr<GraphicsPipeline> mVertexLitPipeline;
};
