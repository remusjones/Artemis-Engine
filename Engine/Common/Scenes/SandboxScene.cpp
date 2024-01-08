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
    mTexturedMeshPipeline = std::make_unique<GraphicsPipeline>("Textured Mesh", mDefaultPipelineConfig);
    mHalfLambertMeshPipeline = std::make_unique<GraphicsPipeline>("Half Lambert", mDefaultPipelineConfig);

    mVertexLitPipeline->CreateShaderModule("/Assets/Shaders/3DVertexLit_v.spv",
                                          VK_SHADER_STAGE_VERTEX_BIT);
    mVertexLitPipeline->CreateShaderModule("/Assets/Shaders/Lit_f.spv",
                                          VK_SHADER_STAGE_FRAGMENT_BIT);

    mUnlitMeshPipeline->CreateShaderModule("/Assets/Shaders/3DUnlit_v.spv",
                                          VK_SHADER_STAGE_VERTEX_BIT);
    mUnlitMeshPipeline->CreateShaderModule("/Assets/Shaders/Lit_f.spv",
                                          VK_SHADER_STAGE_FRAGMENT_BIT);

    mTexturedMeshPipeline->CreateShaderModule("/Assets/Shaders/3DObject_v.spv",
                                             VK_SHADER_STAGE_VERTEX_BIT);
    mTexturedMeshPipeline->CreateShaderModule("/Assets/Shaders/TexturedLit_f.spv",
                                             VK_SHADER_STAGE_FRAGMENT_BIT);

    mHalfLambertMeshPipeline->CreateShaderModule("/Assets/Shaders/3DObject_v.spv",
                                                VK_SHADER_STAGE_VERTEX_BIT);
    mHalfLambertMeshPipeline->CreateShaderModule("/Assets/Shaders/HalfLambert_f.spv",
                                                VK_SHADER_STAGE_FRAGMENT_BIT);


    Material *monkeyMaterial = mVertexLitPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *monkeyTexturedMaterial = mTexturedMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *teapotMaterial = mTexturedMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *lightMaterial = mUnlitMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *sphereMaterial = mTexturedMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();
    Material *halfLambertMaterial = mHalfLambertMeshPipeline->CreateMaterialInstance<DefaultMaterial>().get();

    mVertexLitPipeline->MakeMaterials(0);
    mUnlitMeshPipeline->MakeMaterials(0);
    mTexturedMeshPipeline->MakeMaterials(0);
    mHalfLambertMeshPipeline->MakeMaterials(0);

    // Create Objects, and bind mesh and materials

    mMonkey = new MeshObject();
    mMonkey->CreateObject(*mVertexLitPipeline, *monkeyMaterial, "Monkey Vert");
    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/monkey_smooth.obj")).c_str());

    mMonkey2 = new MeshObject();
    mMonkey2->CreateObject(*mTexturedMeshPipeline, *monkeyTexturedMaterial, "Monkey Lit");
    mMonkey2->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/monkey_smooth.obj")).c_str());

    mTeapot = new MeshObject();
    mTeapot->CreateObject(*mTexturedMeshPipeline, *teapotMaterial,
                          "Teapot");
    mTeapot->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/teapot.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*mUnlitMeshPipeline, *lightMaterial, "Light");
    mLight->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/Assets/Models/sphere.obj")).c_str());

    mSphere = new MeshObject();
    mSphere->CreateObject(*mTexturedMeshPipeline, *sphereMaterial,
                          "Sphere");
    mSphere->LoadMesh((FileManagement::GetWorkingDirectory() +
                       std::string("/Assets/Models/sphere.obj")).c_str());

    mCube = new MeshObject();
    mCube->CreateObject(*mHalfLambertMeshPipeline,
                        *halfLambertMaterial,
                        "Cube Half Lambert");
    mCube->LoadMesh((FileManagement::GetWorkingDirectory() +
                     std::string("/Assets/Models/cube.obj")).c_str());


    // Init positions
    mMonkey->mTransform.SetPosition({-2, 0, -20.0f});
    mMonkey2->mTransform.SetPosition({2, 0, -5.0f});
    mTeapot->mTransform.SetPosition({2, 0, -20.0f});
    mTeapot->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mSphere->mTransform.SetPosition({-5, 0, -5});
    mCube->mTransform.SetPosition({-2, 0, -5});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mLight->mTransform.SetScale({0.2f, 0.2f, 0.2f});

    // Register to scene TODO: Review if we auto-register these
    mObjects.push_back(mMonkey);
    mObjects.push_back(mMonkey2);
    mObjects.push_back(mTeapot);
    mObjects.push_back(mLight);
    mObjects.push_back(mSphere);
    mObjects.push_back(mCube);


    // TODO: Condense into texture create function
    // auto *teapotTexture = new Texture();
    // mLoadedTextures["textureTest"] = teapotTexture;
    //
    // teapotTexture->LoadImageFromDisk( std::string(FileManagement::GetWorkingDirectory() +
    //     "/Assets/Textures/textureTest.png").c_str());
    // teapotTexture->Create();
    // mTeapot->mMaterial->BindTexture(*teapotTexture, 3);
    //
    // auto *sphereAlbedo = new Texture();
    // mLoadedTextures["sphereAlbedo"] = sphereAlbedo;
    // sphereAlbedo->LoadImageFromDisk(std::string(FileManagement::GetWorkingDirectory() +
    //     "/Assets/Textures/Stone Wall.png").c_str());
    // sphereAlbedo->Create();
    // sphereMaterial->BindTexture(*sphereAlbedo, 3);
    //

    auto *stoneTexture = new Texture();
    mLoadedTextures["stoneTexture"] = stoneTexture;
    std::vector<std::string> stoneSet;
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall.png");
    stoneSet.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone Wall_NRM.png");
    stoneTexture->LoadImagesFromDisk(stoneSet);
    stoneTexture->Create();

    teapotMaterial->BindTexture(stoneTexture->mImageBufferInfo, DefaultMaterial::TEXTURE);


    // auto *combinedTexture = new Texture();
    // std::vector<std::string> set;
    // set.push_back(std::string(FileManagement::GetWorkingDirectory() +
    //                           "/Assets/Textures/Stone Wall.png"));
    // set.push_back(std::string(FileManagement::GetWorkingDirectory() +
    //                           "/Assets/Textures/Stone Wall_NRM.png"));
    // //combinedTexture->LoadImagesFromDisk(set);
    // std::vector<Color_RGBA> testColors;
    // testColors.push_back(Color_RGBA(255, 255, 255, 255));
    // testColors.push_back(Color_RGBA(127, 127, 255, 1));
    // combinedTexture->CreateDefault(testColors);
    // //combinedTexture->Create();
    // mLoadedTextures["combinedTexture"] = combinedTexture;
    // testSphereMaterial->BindTexture(*combinedTexture, 3);

    //
    // sphereNormal->Create();
    // sphereMaterial->BindTexture(*sphereNormal, 4);
    //
    // // Supress vulkan validation until I have a default descriptor setter
    // mTeapot->mMaterial->BindTexture(*sphereNormal, 4);
    // mHalfLambertSphere->mMaterial->BindTexture(*sphereAlbedo, 3);
    // mHalfLambertSphere->mMaterial->BindTexture(*sphereNormal, 4);

    mActiveSceneCamera = new FlyCamera();
    mActiveSceneCamera->Construct();
    mActiveSceneCamera->mTransform.SetPosition({0, 0, -5.0f});

    // TODO: Clean allll this up
    mCubemap = new Cubemap();
    mCubemap->Create(nullptr, "Cubemap Material");
    mCubemapMesh = new MeshObject();
    mCubemapMesh->mName = "Skybox";
    mObjects.push_back(mCubemapMesh);
    mCubemapMesh->mMesh = new Mesh();
    mCubemapMesh->LoadMesh((FileManagement::GetWorkingDirectory() +
                           std::string("/Assets/Models/cube.obj")).c_str());
    mCubemapMesh->mMaterial = mCubemap;


    std::vector<VkDescriptorSetLayout> mCubemapLayouts;
    mCubemapLayouts.push_back(mCubemap->GetDescriptorLayout());
    mCubemapPipeline = std::make_unique<CubemapRenderSystem>(mCubemapLayouts);

    AddGraphicsPipeline(mCubemapPipeline->mPipeline.get());
    mCubemapMesh->mGraphicsPipeline = mCubemapPipeline->mPipeline.get();
    mCubemapMesh->mGraphicsPipeline->AddRenderer(mCubemapMesh);

    AddGraphicsPipeline(mVertexLitPipeline.get());
    AddGraphicsPipeline(mUnlitMeshPipeline.get());
    AddGraphicsPipeline(mTexturedMeshPipeline.get());
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
    Scene::Cleanup();

    delete mCubemap;
}
