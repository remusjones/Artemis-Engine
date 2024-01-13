//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "ImGuizmo.h"
#include "Logger.h"

#include "VulkanGraphicsImpl.h"
#include "Physics/PhysicsSystem.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Systems/GraphicsPipeline.h"


void Scene::Construct(const char *aSceneName) {
    mSceneName = aSceneName;

    mPhysicsSystem = new PhysicsSystem();
    mPhysicsSystem->Create();

    for (const auto obj: mObjects) {
        obj->Construct();
    }
    assert(mActiveSceneCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame) {
    for (auto obj: mGraphicsPipelines) {
        obj->Draw(aCommandBuffer, *this);
    }

    OnImGuiRender();
}

// Declaring these here due to circular deps
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void Scene::OnImGuiRender() {
    ImGui::Begin(mSceneName);

    // Draw Gizmo Controls TODO: Add KB Control shortcuts 
    ImGui::SeparatorText("Controls");
    ImGui::BeginChild(GetUniqueLabel("Controls"),
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY); {
        if (ImGui::RadioButton(GetUniqueLabel("Translate"), mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Rotate"), mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Scale"), mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton(GetUniqueLabel("Local"), mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton(GetUniqueLabel("World"), mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
    }
    ImGui::EndChild();

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

    // Draw Objects in scene
    if (ImGui::CollapsingHeader("Objects")) {
        for (const auto obj: mObjects) {
            ImGui::BeginGroup();
            ImGui::Indent(); {
                if (ImGui::CollapsingHeader(obj->GetUniqueLabel(obj->mName))) {
                    ImGui::Indent();

                    // Draw Gizmos
                    glm::mat4 matrix = obj->mTransform.GetWorldMatrix();
                    const ImGuiIO &io = ImGui::GetIO();

                    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                    if (ImGuizmo::Manipulate(glm::value_ptr(mActiveSceneCamera->GetViewMatrix()),
                                             glm::value_ptr(mActiveSceneCamera->GetPerspectiveMatrix()),
                                             mCurrentGizmoOperation, mCurrentGizmoMode,
                                             glm::value_ptr(matrix),NULL)) {
                        obj->mTransform.SetMatrix(matrix);
                    }
                    // Draw Object UI
                    obj->OnImGuiRender();

                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();
            ImGui::EndGroup();
        }
    }


    ImGui::SeparatorText("Statistics");
    ImGui::BeginChild(GetUniqueLabel("Statistics"),
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY); {
        ImGui::Text("FPS: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(gGraphics->GetFps())).c_str());
        ImGui::SameLine();
        ImGui::PlotLines("##fpsHistory", &gGraphics->GetFpsHistory().mBuffer.front(),
                         gGraphics->GetFpsHistory().mBuffer.size());
        ImGui::Text("Delta Time: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(gGraphics->DeltaTimeUnscaled()).c_str());

        ImGui::Text("Objects: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(mObjects.size())).c_str());
        ImGui::Text("Graphic Systems: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(mGraphicsPipelines.size())).c_str());
        if (ImGui::CollapsingHeader("Graphic Systems Info")) {
            ImGui::Indent();
            for (const auto pipeline: mGraphicsPipelines) {
                if (ImGui::CollapsingHeader(pipeline->mPipelineName)) {
                    ImGui::Indent();
                    ImGui::Text("Material Count: ");
                    ImGui::SameLine(); ImGui::Text(std::to_string(pipeline->mRenderers.size()).c_str());
                    for (const auto renderer: pipeline->mRenderers) {
                        ImGui::Text(renderer->mMaterial->mMaterialName);
                    }
                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}


void Scene::Tick(const float aDeltaTime) {

    mPhysicsSystem->Tick(aDeltaTime);
    for (const auto obj: mObjects) {
        obj->Tick(aDeltaTime);
    }
}

void Scene::Cleanup() {
    Logger::Log(spdlog::level::info, (std::string("Cleaning up scene ") + mSceneName).c_str());
    for (const auto obj: mObjects) {
        obj->Cleanup();
        delete obj;
    }

    delete mActiveSceneCamera;
    for (const auto pipeline: mGraphicsPipelines) {
        pipeline->Destroy();
        delete pipeline;
    }
    mPhysicsSystem->Destroy();
    delete mPhysicsSystem;
    mPhysicsSystem = nullptr;
}

void Scene::AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline) {

    mGraphicsPipelines.push_back(aGraphicsPipeline);
    aGraphicsPipeline->Create();
}
