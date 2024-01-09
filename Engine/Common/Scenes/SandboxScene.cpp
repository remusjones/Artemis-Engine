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
#include "Vulkan/Systems/CubemapRenderSystem.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SandboxScene::Construct(const char *aSceneName) {
    GraphicsPipeline::DefaultPipelineConfigInfo(mDefaultPipelineConfig);
    mVertexLitPipeline = std::make_unique<GraphicsPipeline>("Vertex Lit Mesh", mDefaultPipelineConfig);
    mUnlitMeshPipeline = std::make_unique<GraphicsPipeline>("Unlit Mesh", mDefaultPipelineConfig);
    mTexturedMeshPipeline = std::make_shared<RenderSystemBase>();
    mCubemapPipeline = std::make_unique<CubemapRenderSystem>();

    mHalfLambertMeshPipeline = std::make_unique<GraphicsPipeline>("Half Lambert", mDefaultPipelineConfig);

    mVertexLitPipeline->CreateShaderModule("/Assets/Shaders/3DVertexLit_v.spv",
                                           VK_SHADER_STAGE_VERTEX_BIT);
    mVertexLitPipeline->CreateShaderModule("/Assets/Shaders/Lit_f.spv",
                                           VK_SHADER_STAGE_FRAGMENT_BIT);

    mUnlitMeshPipeline->CreateShaderModule("/Assets/Shaders/3DUnlit_v.spv",
                                           VK_SHADER_STAGE_VERTEX_BIT);
    mUnlitMeshPipeline->CreateShaderModule("/Assets/Shaders/Lit_f.spv",
                                           VK_SHADER_STAGE_FRAGMENT_BIT);

    //mTexturedMeshPipeline->CreateShaderModule("/Assets/Shaders/3DObject_v.spv",
    //                                          VK_SHADER_STAGE_VERTEX_BIT);
    //mTexturedMeshPipeline->CreateShaderModule("/Assets/Shaders/TexturedLit_f.spv",
    //                                          VK_SHADER_STAGE_FRAGMENT_BIT);

    mHalfLambertMeshPipeline->CreateShaderModule("/Assets/Shaders/3DObject_v.spv",
                                                 VK_SHADER_STAGE_VERTEX_BIT);
    mHalfLambertMeshPipeline->CreateShaderModule("/Assets/Shaders/HalfLambert_f.spv",
                                                 VK_SHADER_STAGE_FRAGMENT_BIT);


    Material *monkeyMaterial = mVertexLitPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *monkeyTexturedMaterial = mTexturedMaterialFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *teapotMaterial = mTexturedMaterialFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *lightMaterial = mUnlitMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *sphereMaterial = mTexturedMaterialFactory.CreateMaterialInstance<DefaultMaterial>().get();
    Material *halfLambertMaterial = mHalfLambertMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    mCubemap = mGenericMaterialFactory.CreateMaterialInstance<Cubemap>();

    mVertexLitPipeline->MakeMaterials(0);
    mUnlitMeshPipeline->MakeMaterials(0);
    mTexturedMaterialFactory.MakeMaterials();
    mGenericMaterialFactory.MakeMaterials();
    mHalfLambertMeshPipeline->MakeMaterials(0);

    mTexturedMeshPipeline->Create(mTexturedMaterialFactory.GetDescriptorLayouts());

    std::vector<VkDescriptorSetLayout> mCubemapLayouts;
    mCubemapLayouts.push_back(mCubemap->GetDescriptorLayout());
    mCubemapPipeline->Create(mCubemapLayouts);

    mMonkey = new MeshObject();
    mMonkey->CreateObject(*monkeyMaterial, "Monkey Vert");
    mMonkey->BindRenderer(*mVertexLitPipeline);

    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/monkey_smooth.obj")).c_str());

    mMonkey2 = new MeshObject();
    mMonkey2->CreateObject(*monkeyTexturedMaterial, "Monkey Lit");
    mMonkey2->BindRenderer(*mTexturedMeshPipeline->mPipeline);
    mMonkey2->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/monkey_smooth.obj")).c_str());

    mTeapot = new MeshObject();
    mTeapot->CreateObject(*teapotMaterial,
                          "Teapot");
    mTeapot->BindRenderer(*mTexturedMeshPipeline->mPipeline);

    mTeapot->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/teapot.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*lightMaterial, "Light");
    mLight->BindRenderer(*mUnlitMeshPipeline);
    mLight->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/sphere.obj")).c_str());

    mSphere = new MeshObject();
    mSphere->CreateObject(*sphereMaterial,
                          "Sphere");
    mSphere->BindRenderer(*mTexturedMeshPipeline->mPipeline);
    mSphere->LoadMesh((FileManagement::GetWorkingDirectory() +
                       std::string("/Assets/Models/sphere.obj")).c_str());

    mCube = new MeshObject();
    mCube->CreateObject(*halfLambertMaterial,
                        "Cube Half Lambert");
    mCube->BindRenderer(*mHalfLambertMeshPipeline);

    mCube->LoadMesh((FileManagement::GetWorkingDirectory() +
    std::string("/Assets/Models/cube.obj")).c_str(),
    (FileManagement::GetWorkingDirectory() + std::string("/Assets/Models/")).c_str());


    // Init positions
    mMonkey->mTransform.SetPosition({-2, 0, -20.0f});
    mMonkey2->mTransform.SetPosition({2, 0, -5.0f});
    mTeapot->mTransform.SetPosition({2, 0, -20.0f});
    mTeapot->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mSphere->mTransform.SetPosition({-5, 0, -5});
    mCube->mTransform.SetPosition({-2, 0, -5});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mLight->mTransform.SetScale({0.2f, 0.2f, 0.2f});


    auto *stoneTexture = new Texture();
    mLoadedTextures["stoneTexture"] = stoneTexture;
    std::vector<std::string> stoneSet;
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall.png");
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall_NRM.png");
    stoneTexture->LoadImagesFromDisk(stoneSet);
    stoneTexture->Create();

    teapotMaterial->BindTexture(stoneTexture->mImageBufferInfo, DefaultMaterial::TEXTURE);


    mCubemapMesh = new MeshObject();
    mCubemapMesh->CreateObject(*mCubemap, "Skybox");
    mCubemapMesh->BindRenderer(*mCubemapPipeline->mPipeline);
    mCubemapMesh->LoadMesh((FileManagement::GetWorkingDirectory() +
                            std::string("/Assets/Models/cube.obj")).c_str());

    mActiveSceneCamera = new FlyCamera();
    mActiveSceneCamera->Construct();
    mActiveSceneCamera->mTransform.SetPosition({0, 0, -5.0f});


    // Register to scene TODO: Review if we auto-register these
    mObjects.push_back(mMonkey);
    mObjects.push_back(mMonkey2);
    mObjects.push_back(mTeapot);
    mObjects.push_back(mLight);
    mObjects.push_back(mSphere);
    mObjects.push_back(mCube);
    mObjects.push_back(mCubemapMesh);


    AddGraphicsPipeline(mVertexLitPipeline.get());
    AddGraphicsPipeline(mUnlitMeshPipeline.get());
    AddGraphicsPipeline(mCubemapPipeline->mPipeline.get());
    AddGraphicsPipeline(mTexturedMeshPipeline->mPipeline.get());
    AddGraphicsPipeline(mHalfLambertMeshPipeline.get());
    Scene::Construct(aSceneName);
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    mLight->mMaterial->mMaterialProperties.mColor =
            glm::vec4(mSceneData.color.x, mSceneData.color.y, mSceneData.color.z, 1);

    mSceneData.position = mLight->mTransform.Position();
    mSceneData.mViewMatrix = mActiveSceneCamera->GetViewMatrix();
    mSceneData.mViewPos = glm::vec4(mActiveSceneCamera->mTransform.Position(), 1.0f);
    mSceneData.mViewProjectionMatrix = mActiveSceneCamera->GetPerspectiveMatrix();

    mMonkey->mTransform.RotateAxis(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    mMonkey2->mTransform.RotateAxis(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    Scene::Tick(aDeltaTime);

    // Late Tick ..
    mActiveSceneCamera->Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    for (auto loadedTextures: mLoadedTextures) {
        loadedTextures.second->Destroy();
        delete loadedTextures.second;
    }
    mTexturedMaterialFactory.DestroyMaterials();
    mGenericMaterialFactory.DestroyMaterials();
    Scene::Cleanup();
}
