//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <unordered_map>

#include "Scene.h"
#include "Base/Common/Buffers/Texture.h"


class SandboxScene final : public Scene {
public:
    void Construct(const char *aSceneName) override;

    void Tick(float aDeltaTime) override;

    void Cleanup() override;

private:
    std::unordered_map<std::string, Texture *> mLoadedTextures;

    VkSampler mBlockySampler;
    MeshObject *mMonkey;
    MeshObject *mMonkey2;
    MeshObject *mTeapot;
    MeshObject *mLight;
    MeshObject *mSphere;
    MeshObject *mCube;
};
