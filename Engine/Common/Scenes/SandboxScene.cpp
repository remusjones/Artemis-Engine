//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"
#include "VulkanGraphicsImpl.h"
#include "File Management/FileManagement.h"
#include "glog/logging.h"
#include "Objects/Camera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"

void SandboxScene::Construct(const char *aSceneName) {
    // Create basic mesh pipeline
    GraphicsPipeline *meshPipeline = new GraphicsPipeline("Mesh Pipeline");

    meshPipeline->AddShader("/Shaders/3DObject_v.spv",
                            VK_SHADER_STAGE_VERTEX_BIT);
    meshPipeline->AddShader("/Shaders/VertexLit_f.spv",
                            VK_SHADER_STAGE_FRAGMENT_BIT);

    mMonkey = new MeshObject();
    mMonkey->CreateObject(*meshPipeline, "Monkey");
    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_smooth.obj")).c_str());


    mMonkey2 = new MeshObject();
    mMonkey2->CreateObject(*meshPipeline, "Monkey2");
    mMonkey2->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_flat.obj")).c_str());

    mMonkey->mTransform.SetPosition({-2, 0, 0});
    mMonkey2->mTransform.SetPosition({2, 0, 0});

    mObjects.push_back(mMonkey);
    mObjects.push_back(mMonkey2);


    mActiveCamera = new Camera();
    mActiveCamera->mTransform.SetPosition({0, 0, -5.0f});


    AddGraphicsPipeline(meshPipeline);
    Scene::Construct(aSceneName);
}

float deltaAccumulated;
void SandboxScene::Tick(float aDeltaTime) {
    mMonkey->mTransform.mTransformationMatrix = glm::rotate
    (mMonkey->mTransform.mTransformationMatrix,
     aDeltaTime/10,
     glm::vec3(0.0f, 1.0f, 1.0f));

    deltaAccumulated += aDeltaTime/3;
    mSceneLightingData.position.x = 2 * std::sin(2 * M_PI * 1 * deltaAccumulated);
    mSceneLightingData.position.y = 1.0f;  // Keep it constant or modify as needed

    mMonkey2->mTransform.mTransformationMatrix = glm::rotate
    (mMonkey2->mTransform.mTransformationMatrix,
     aDeltaTime,
     glm::vec3(0.0f, 0.8f, -1.0f));


    Scene::Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    Scene::Cleanup();
}
