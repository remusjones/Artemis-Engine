//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"

#include "imgui.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorldImporter.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "Components/Collision/ColliderComponent.h"
#include "File Management/FileManagement.h"
#include "Objects/Camera.h"
#include "Objects/FlyCamera.h"
#include "Physics/PhysicsSystem.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Common/Primative.h"
#include "Vulkan/Renderers/SkyboxRenderer.h"
#include "Vulkan/Materials/Cubemap.h"
#include "Vulkan/Materials/DefaultMaterial.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SandboxScene::PreConstruct(const char *aSceneName) {
    Scene::PreConstruct(aSceneName);
}

void SandboxScene::Construct() {
    //
    // Create Render Pipelines
    //

    GraphicsPipeline::DefaultPipelineConfigInfo(mDefaultPipelineConfig);
    mPBRPipeline = std::make_shared<PBRRenderSystem>();
    mUnlitPipeline = std::make_shared<UnlitRenderSystem>();
    mCubemapPipeline = std::make_unique<SkyboxRenderSystem>();

    //
    // Define Material Usages
    //
    Material *monkeyTexturedMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>("Monkey PBR").get();
    Material *teapotMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>("Teapot PBR").get();
    Material *lightMaterial = mMaterialUnlitFactory.CreateMaterialInstance<DefaultMaterial>("Light Unlit").get();
    Material *sphereMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>("Sphere PBR").get();
    Material *cubeMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>("Cube PBR").get();
    mCubemap = mGenericMaterialFactory.CreateMaterialInstance<Cubemap>("Skybox Cubemap");

    //
    // Make Materials
    //
    mMaterialUnlitFactory.MakeMaterials();
    mMaterialPBRFactory.MakeMaterials();
    mGenericMaterialFactory.MakeMaterials();

    //
    // Bind Materials
    //
    mUnlitPipeline->Create(mMaterialUnlitFactory.GetDescriptorLayouts());
    mPBRPipeline->Create(mMaterialPBRFactory.GetDescriptorLayouts());

    std::vector<VkDescriptorSetLayout> mCubemapLayouts;
    mCubemapLayouts.push_back(mCubemap->GetDescriptorLayout());
    mCubemapPipeline->Create(mCubemapLayouts);

    //
    // Create Objects
    // TODO: Create constructor helper to make this smaller?
    //
    mMonkey = new MeshObject();
    mMonkey->CreateObject(*monkeyTexturedMaterial, "Monkey Lit");
    mMonkey->mMeshRenderer.BindRenderer(*mPBRPipeline->mPipeline);
    mMonkey->mMeshRenderer.LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/monkey_smooth.obj")).c_str());

    mTeapot = new MeshObject();
    mTeapot->CreateObject(*teapotMaterial,
                          "Teapot");
    mTeapot->mMeshRenderer.BindRenderer(*mPBRPipeline->mPipeline);

    mTeapot->mMeshRenderer.LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/teapot.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*lightMaterial, "Light");
    mLight->mMeshRenderer.BindRenderer(*mUnlitPipeline->mPipeline);
    mLight->mMeshRenderer.LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/sphere.obj")).c_str());


    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            constexpr float sphereRadius = 0.5f;
            MeshObject* sphere = MakeObject("Physics Sphere",
                                            "/Assets/Models/sphere.obj", *sphereMaterial,
                                            *mPBRPipeline->mPipeline, glm::vec3(i, 0, j), glm::vec3(0),
                                            glm::vec3(sphereRadius));

            AttachSphereCollider(*sphere, sphereRadius, 0.25f);
        }
    }


    // btCollisionShape* colShape = new btSphereShape(static_cast<btScalar>(1.));
    // mPhysicsSystem->mCollisionShapes.push_back(colShape);

    const glm::vec3 floorScale(25, 0.5f, 25);
    mFloor = MakeObject("Floor", "/Assets/Models/cube.obj",
        *cubeMaterial, *mPBRPipeline->mPipeline,
    glm::vec3(0, -10, 0), glm::vec3(0), floorScale);

    AttachBoxCollider(*mFloor, floorScale, 0);
    //auto *floorCollider = new ColliderComponent();
    //ColliderCreateInfo floorColliderInfo{};
    //floorColliderInfo.collisionShape = new btBoxShape(btVector3(floorScale.x, floorScale.y, floorScale.z));
    //floorColliderInfo.mass = 0.f;
    //floorCollider->Create(mPhysicsSystem, floorColliderInfo);
    //mFloor->AddComponent(floorCollider);

    //
    // Create Teapot Textures
    // TODO: Create Texture factory?
    //
    auto *stoneTexture = new Texture();
    mLoadedTextures["stoneTexture"] = stoneTexture;
    std::vector<std::string> stoneSet;
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall.png");
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall_NRM.png");
    stoneTexture->LoadImagesFromDisk(stoneSet);
    stoneTexture->Create();

    teapotMaterial->BindTexture(stoneTexture->mImageBufferInfo, DefaultMaterial::TEXTURE);

    //
    // Skybox TODO: Skybox Constructor Required
    //
    mCubemapMesh = new Primative("Skybox");
    SkyboxRenderer *mSkyboxRenderer = new SkyboxRenderer();
    mCubemapMesh->mRenderer = mSkyboxRenderer;
    mSkyboxRenderer->mMaterial = mCubemap.get();
    mSkyboxRenderer->mTransform = &mCubemapMesh->mTransform;
    mSkyboxRenderer->BindRenderer(*mCubemapPipeline->mPipeline);
    mSkyboxRenderer->LoadMesh((FileManagement::GetWorkingDirectory() +
                               std::string("/Assets/Models/cube.obj")).c_str());

    //
    // Scene Camera
    //
    mActiveSceneCamera = new FlyCamera();
    mActiveSceneCamera->Construct();
    mActiveSceneCamera->mTransform.SetPosition({0, 0, -5.0f});


    //
    // Setup Scene Dependencies
    // Register to scene TODO: Review if we auto-register these
    //
    mObjects.push_back(mMonkey);
    mObjects.push_back(mTeapot);
    mObjects.push_back(mLight);
    mObjects.push_back(mCubemapMesh);

    //
    // Set Default Positions
    //
    mMonkey->mTransform.SetPosition({2, 0, -5.0f});
    mTeapot->mTransform.SetPosition({2, 0, -20.0f});
    mTeapot->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mLight->mTransform.SetScale({0.2f, 0.2f, 0.2f});

    //
    // Setup Draw Order
    //
    AddGraphicsPipeline(mCubemapPipeline->mPipeline.get());
    AddGraphicsPipeline(mUnlitPipeline->mPipeline.get());
    AddGraphicsPipeline(mPBRPipeline->mPipeline.get());

    //
    // Construct Scene
    //
    Scene::Construct();
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    mLight->mMeshRenderer.mMaterial->mMaterialProperties.mColor =
            glm::vec4(mSceneData.color.x, mSceneData.color.y, mSceneData.color.z, 1);

    mSceneData.position = mLight->mTransform.Position();
    mSceneData.mViewMatrix = mActiveSceneCamera->GetViewMatrix();
    mSceneData.mViewPos = glm::vec4(mActiveSceneCamera->mTransform.Position(), 1.0f);
    mSceneData.mViewProjectionMatrix = mActiveSceneCamera->GetPerspectiveMatrix();

    mMonkey->mTransform.RotateAxis(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    Scene::Tick(aDeltaTime);

    mActiveSceneCamera->Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    for (auto loadedTextures: mLoadedTextures) {
        loadedTextures.second->Destroy();
        delete loadedTextures.second;
    }

    mMaterialUnlitFactory.DestroyMaterials();
    mMaterialPBRFactory.DestroyMaterials();
    mGenericMaterialFactory.DestroyMaterials();
    Scene::Cleanup();
}

void SandboxScene::OnImGuiRender() {
    if (ImGui::Button(GetUniqueLabel("Rebuild PBR Pipeline"))) {
        gGraphics->mVulkanEngine.SubmitEndOfFrameTask([&] {
            vkDeviceWaitIdle(gGraphics->mLogicalDevice);
            // Find iter of our Render System
            // Destroy it
            mPBRPipeline->mPipeline->Destroy();
            // Recreate it
            mPBRPipeline->Create(mPBRPipeline->GetBoundDescriptors());
            mPBRPipeline->mPipeline->Create();
        });
    }
    Scene::OnImGuiRender();
}

MeshObject *SandboxScene::MakeObject(const char* aName, const char* aMeshPath, Material &aMaterial,
                                     GraphicsPipeline &aPipeline, const glm::vec3 aPos, const glm::vec3 aRot,
                                     const glm::vec3 aScale) {
    auto *object = new MeshObject();

    object->CreateObject(aMaterial, aName);
    object->mMeshRenderer.BindRenderer(aPipeline);
    object->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() + aMeshPath).c_str());

    object->mTransform.SetPosition(aPos);
    object->mTransform.SetRotation(aRot);
    object->mTransform.SetScale(aScale);

    mObjects.push_back(object);

    return object;
}

void SandboxScene::AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass) const {
    auto* sphereCollider = new ColliderComponent();
    ColliderCreateInfo sphereColliderInfo{};
    sphereColliderInfo.collisionShape = new btSphereShape(aRadius);
    sphereColliderInfo.mass = aMass;
    sphereCollider->Create(mPhysicsSystem, sphereColliderInfo);
    aEntity.AddComponent(sphereCollider);
}

void SandboxScene::AttachBoxCollider(Entity &aEntity, glm::vec3 aHalfExtents, float aMass) const {
    auto* boxCollider = new ColliderComponent();
    ColliderCreateInfo boxColliderInfo{};
    boxColliderInfo.collisionShape = new btBoxShape(btVector3(aHalfExtents.x, aHalfExtents.y, aHalfExtents.z));
    boxColliderInfo.mass = aMass;
    boxCollider->Create(mPhysicsSystem, boxColliderInfo);
    aEntity.AddComponent(boxCollider);
}
