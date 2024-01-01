//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"

#include "LoadUtilities.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"
#include "File Management/FileManagement.h"
#include "Objects/Camera.h"
#include "Objects/FlyCamera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void SandboxScene::Construct(const char *aSceneName) {
    // Create basic mesh pipeline
    GraphicsPipeline *vertexLitPipeline = new GraphicsPipeline("Lit Mesh Pipeline");
    GraphicsPipeline *unlitMeshPipeline = new GraphicsPipeline("Unlit Mesh Pipeline");
    GraphicsPipeline *texturedMeshPipeline = new GraphicsPipeline("Textured Mesh Pipeline");
    GraphicsPipeline *halfLambertMeshPipeline = new GraphicsPipeline("Half Lambert Mesh Pipeline");

    vertexLitPipeline->AddShader("/Shaders/3DVertexLit_v.spv",
                                 VK_SHADER_STAGE_VERTEX_BIT);
    vertexLitPipeline->AddShader("/Shaders/Lit_f.spv",
                                 VK_SHADER_STAGE_FRAGMENT_BIT);


    unlitMeshPipeline->AddShader("/Shaders/3DUnlit_v.spv",
                                 VK_SHADER_STAGE_VERTEX_BIT);
    unlitMeshPipeline->AddShader("/Shaders/Lit_f.spv",
                                 VK_SHADER_STAGE_FRAGMENT_BIT);

    texturedMeshPipeline->AddShader("/Shaders/3DObject_v.spv",
                                    VK_SHADER_STAGE_VERTEX_BIT);
    texturedMeshPipeline->AddShader("/Shaders/TexturedLit_f.spv",
                                    VK_SHADER_STAGE_FRAGMENT_BIT);

    halfLambertMeshPipeline->AddShader("/Shaders/3DObject_v.spv",
                                    VK_SHADER_STAGE_VERTEX_BIT);
    halfLambertMeshPipeline->AddShader("/Shaders/HalfLambert_f.spv",
                                    VK_SHADER_STAGE_FRAGMENT_BIT);

    mMonkey = new MeshObject();
    mMonkey->CreateObject(*vertexLitPipeline, "Monkey");
    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_smooth.obj")).c_str());


    mTeapot = new MeshObject();
    mTeapot->CreateObject(*texturedMeshPipeline, "Teapot");
    mTeapot->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/teapot.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*unlitMeshPipeline, "Light");
    mLight->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/sphere.obj")).c_str());

    mSphere = new MeshObject();
    mSphere->CreateObject(*texturedMeshPipeline, "Sphere");
    mSphere->LoadMesh((FileManagement::GetWorkingDirectory() +
                       std::string("/../Models/sphere.obj")).c_str());

    mHalfLambertSphere = new MeshObject();
    mHalfLambertSphere->CreateObject(*halfLambertMeshPipeline, "Sphere Lambert");
    mHalfLambertSphere->LoadMesh((FileManagement::GetWorkingDirectory() +
                                  std::string("/../Models/sphere.obj")).c_str());


    mMonkey->mTransform.SetPosition({-2, 0, -20.0f});
    mTeapot->mTransform.SetPosition({2, 0, -20.0f});
    mTeapot->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mSphere->mTransform.SetPosition({2, 0, -5});
    mHalfLambertSphere->mTransform.SetPosition({-2, 0, -5});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});
    mLight->mTransform.SetScale({0.2f, 0.2f, 0.2f});

    mObjects.push_back(mMonkey);
    mObjects.push_back(mTeapot);
    mObjects.push_back(mLight);
    mObjects.push_back(mSphere);
    mObjects.push_back(mHalfLambertSphere);


    // TODO: Condense into texture create function
    auto *teapotTexture = new Texture();
    mLoadedTextures["textureTest"] = teapotTexture;

    teapotTexture->LoadImageFromDisk("../Textures/textureTest.png");
    teapotTexture->Create();
    mTeapot->mMaterial->BindTexture(*teapotTexture, 3);

    auto *sphereAlbedo = new Texture();
    mLoadedTextures["sphereAlbedo"] = sphereAlbedo;
    sphereAlbedo->LoadImageFromDisk("../Textures/Stone/Stone Wall.png");
    sphereAlbedo->Create();
    mSphere->mMaterial->BindTexture(*sphereAlbedo, 3);

    auto *sphereNormal = new Texture();
    mLoadedTextures["sphereNormal"] = sphereNormal;
    sphereNormal->LoadImageFromDisk("../Textures/Stone/Stone Wall_NRM.png");
    sphereNormal->Create();
    mSphere->mMaterial->BindTexture(*sphereNormal, 4);

    // Supress vulkan validation until I have a default descriptor setter
    mTeapot->mMaterial->BindTexture(*sphereNormal, 4);
    mHalfLambertSphere->mMaterial->BindTexture(*sphereAlbedo, 3);
    mHalfLambertSphere->mMaterial->BindTexture(*sphereNormal, 4);

    mActiveSceneCamera = new FlyCamera();
    mActiveSceneCamera->Construct();
    mActiveSceneCamera->mTransform.SetPosition({0, 0, -5.0f});

    AddGraphicsPipeline(vertexLitPipeline);
    AddGraphicsPipeline(unlitMeshPipeline);
    AddGraphicsPipeline(texturedMeshPipeline);
    AddGraphicsPipeline(halfLambertMeshPipeline);

    Scene::Construct(aSceneName);
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    float yPosition = 1.0f * sin(2 * M_PI * 1 * deltaAccumulated);

    mLight->mTransform.SetPosition({0, yPosition, 0.0f});
    mLight->mMaterial->mMaterialProperties.mColor =
            glm::vec4(mSceneData.color.x, mSceneData.color.y, mSceneData.color.z, 1);

    mSceneData.position = mLight->mTransform.Position();

    mMonkey->mTransform.RotateAxis(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    // mTeapot->mTransform.RotateAround(aDeltaTime / 10, glm::vec3(0.0f, 1.0f, 1.0f));
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
}
