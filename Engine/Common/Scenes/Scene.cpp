//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"

#include "imgui.h"
#include "VulkanGraphicsImpl.h"
#include "glog/logging.h"
#include "Objects/Camera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"


void Scene::Construct(const char *aSceneName) {
    mSceneName = aSceneName;
    for (const auto obj: mObjects) {
        obj->Construct();
    }
    assert(mActiveSceneCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame) {
    for (const auto obj: mGraphicsPipelines) {
        obj->Draw(aCommandBuffer, *this);
    }

    OnImGuiRender();
}

void Scene::OnImGuiRender() {
    ImGui::Begin(mSceneName);

    if (ImGui::CollapsingHeader("Scene Information")) {
        ImGui::Indent();
        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::ColorEdit3(GetUniqueLabel("Point Light Color"), &mSceneData.color[0]);
            ImGui::DragFloat(GetUniqueLabel("Point Light Intensity"), &mSceneData.lightIntensity, 0.0125f);
            ImGui::DragFloat(GetUniqueLabel("Ambient Lighting"), &mSceneData.ambientStrength, 0.1f);
        }
        if (ImGui::CollapsingHeader("Camera")) {
            mActiveSceneCamera->OnImGuiRender();
        }
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Objects")) {
        for (const auto obj: mObjects) {
            ImGui::BeginGroup();
            ImGui::Indent();
            obj->OnImGuiRender();
            ImGui::Unindent();
            ImGui::EndGroup();
        }
    }

    ImGui::End();
}

void Scene::Tick(float aDeltaTime) {
    for (const auto obj: mObjects) {
        obj->Tick(aDeltaTime);
    }
}

void Scene::Cleanup() {
    LOG(INFO) << "Cleaning up scene " << mSceneName;
    for (const auto obj: mObjects) {
        obj->Cleanup();
        delete obj;
    }

    delete mActiveSceneCamera;
    for (const auto pipeline: mGraphicsPipelines) {
        pipeline->Destroy();
        delete pipeline;
    }
}

void Scene::AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline) {
    LOG(INFO) << "Creating Graphics Pipeline: " << aGraphicsPipeline->
            mPipelineName;
    mGraphicsPipelines.push_back(aGraphicsPipeline);
    aGraphicsPipeline->Create();
}
