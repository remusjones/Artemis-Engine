//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Cubemap.h"
#include "Base/Common/DefaultMaterial.h"
#include "Base/Common/Material.h"
#include "File Management/FileManagement.h"
#include "Objects/Camera.h"
#include "Objects/FlyCamera.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Common/Primative.h"
#include "Vulkan/Renderers/SkyboxRenderer.h"
#include "Vulkan/Systems/CubemapRenderSystem.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SandboxScene::Construct(const char *aSceneName) {
    GraphicsPipeline::DefaultPipelineConfigInfo(mDefaultPipelineConfig);

    //
    // Create Pipelines
    //
    mPBRPipeline = std::make_shared<PBRRenderSystem>();
    mUnlitPipeline = std::make_shared<UnlitRenderSystem>();
    mCubemapPipeline = std::make_unique<CubemapRenderSystem>();

    //
    // Define Material Usages
    //
    Material *monkeyTexturedMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *teapotMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *lightMaterial = mMaterialUnlitFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *sphereMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *cubeMaterial = mMaterialPBRFactory.CreateMaterialInstance<DefaultMaterial>().get();
    mCubemap = mGenericMaterialFactory.CreateMaterialInstance<Cubemap>();

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

    mSphere = new MeshObject();
    mSphere->CreateObject(*sphereMaterial,
                          "Sphere");
    mSphere->mMeshRenderer.BindRenderer(*mPBRPipeline->mPipeline);
    mSphere->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() +
                                     std::string("/Assets/Models/sphere.obj")).c_str());

    mCube = new MeshObject();
    mCube->CreateObject(*cubeMaterial,
                        "Cube Half Lambert");
    mCube->mMeshRenderer.BindRenderer(*mPBRPipeline->mPipeline);

    mCube->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() +
                                   std::string("/Assets/Models/cube.obj")).c_str(),
                                  (FileManagement::GetWorkingDirectory() + std::string("/Assets/Models/")).c_str());

    //
    // Create Teapot Textures
    // TODO: Create Texture factory?
    //
    auto *stoneTexture = new Texture();
    mLoadedTextures["stoneTexture"] = stoneTexture;
    std::vector<std::string> stoneSet;
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall.png");
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall_NRM.png");
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
    mObjects.push_back(mSphere);
    mObjects.push_back(mCube);
    mObjects.push_back(mCubemapMesh);

    //
    // Set Default Positions
    //
    mMonkey->mTransform.SetPosition({2, 0, -5.0f});
    mTeapot->mTransform.SetPosition({2, 0, -20.0f});
    mTeapot->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mSphere->mTransform.SetPosition({-5, 0, -5});
    mCube->mTransform.SetPosition({-2, 0, -5});
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
    Scene::Construct(aSceneName);
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
