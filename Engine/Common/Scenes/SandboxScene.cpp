//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"
#include "File Management/FileManagement.h"
#include "glog/logging.h"
#include "Objects/Camera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"

void SandboxScene::Construct(const char *aSceneName) {
    // Create basic mesh pipeline
    GraphicsPipeline *litMeshPipeline = new GraphicsPipeline("Lit Mesh Pipeline");
    GraphicsPipeline *unlitMeshPipeline = new GraphicsPipeline("Unlit Mesh Pipeline");

    litMeshPipeline->AddShader("/Shaders/3DObject_v.spv",
                            VK_SHADER_STAGE_VERTEX_BIT);
    litMeshPipeline->AddShader("/Shaders/Lit_f.spv",
                            VK_SHADER_STAGE_FRAGMENT_BIT);


    unlitMeshPipeline->AddShader("/Shaders/3DUnlit_v.spv",
                            VK_SHADER_STAGE_VERTEX_BIT);
    unlitMeshPipeline->AddShader("/Shaders/Lit_f.spv",
                            VK_SHADER_STAGE_FRAGMENT_BIT);


    mMonkey = new MeshObject();
    mMonkey->CreateObject(*litMeshPipeline, "Monkey");
    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_smooth.obj")).c_str());


    mMonkey2 = new MeshObject();
    mMonkey2->CreateObject(*litMeshPipeline, "Monkey2");
    mMonkey2->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_flat.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*unlitMeshPipeline, "Light");
    mLight->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/sphere.obj")).c_str());


    mMonkey->mTransform.SetPosition({-2, 0, 0});
    mMonkey2->mTransform.SetPosition({2, 0, 0});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});

    mObjects.push_back(mMonkey);
    mObjects.push_back(mMonkey2);
    mObjects.push_back(mLight);


    mActiveCamera = new Camera();
    mActiveCamera->mTransform.SetPosition({0, 0, -5.0f});


    AddGraphicsPipeline(litMeshPipeline);
    AddGraphicsPipeline(unlitMeshPipeline);
    Scene::Construct(aSceneName);
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    float yPosition = 1.0f * sin(2 * M_PI * 1 * deltaAccumulated);

    mLight->mTransform.SetPosition({0, yPosition, 0.0f});
    mLight->mMaterial->mMaterialProperties.mColor =
    glm::vec4(mSceneData.color.x,mSceneData.color.y,mSceneData.color.z,1);

    mSceneData.position = mLight->mTransform.Position();

    mMonkey->mTransform.RotateAround(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    mMonkey2->mTransform.RotateAround(aDeltaTime / 10, glm::vec3(0.0f, 1.0f, 1.0f));


    Scene::Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    Scene::Cleanup();
}
