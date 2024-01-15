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
    MeshObject *MakeObject(const char* aName,
                           const char* aMeshPath,
                           Material &aMaterial,
                           GraphicsPipeline &aPipeline,
                           glm::vec3 aPos = glm::vec3(0),
                           glm::vec3 aRot = glm::vec3(0),
                           glm::vec3 aScale = glm::vec3(1)
    );

    void AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass, float aFriction = 0.5f) const;
    void AttachBoxCollider(::Entity &aEntity, glm::vec3 aHalfExtents, float aMass, float aFriction = 0.5f) const;


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
