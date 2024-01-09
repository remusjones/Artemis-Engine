//
// Created by Remus on 17/12/2023.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MeshObject.h"

#include "imgui.h"
#include "Logger.h"
#include "Base/Common/Material.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Data/Mesh.h"
#include "Scenes/Scene.h"
#include <glm/gtc/type_ptr.hpp>

#include "Vulkan/Systems/GraphicsPipeline.h"

void MeshObject::Construct() {
    Super::Construct();
}

void MeshObject::Tick(float aDeltaTime) {
    Super::Tick(aDeltaTime);
}

void MeshObject::Cleanup() {
    mMeshRenderer.DestroyRenderer();
    Super::Cleanup();
}

void MeshObject::CreateObject(
    Material &aMaterial,
    const char *aName) {
    mName = aName;
    Logger::Log(spdlog::level::info, (std::string("Creating object ") + mName).c_str());
    mMeshRenderer.mMaterial = &aMaterial;
    mMeshRenderer.mTransform = &mTransform;
}

void MeshObject::OnImGuiRender() {
    ImGui::SeparatorText("Transform");
    glm::vec3 pos = mTransform.Position();
    glm::vec3 rot = mTransform.Euler();
    glm::vec3 scale = mTransform.Scale();

    if (ImGui::DragFloat3(GetUniqueLabel("Position"), &pos[0], 0.1f)) {
        mTransform.SetPosition(pos);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Rotation"), &rot[0], 0.1f)) {
        mTransform.SetRotation(rot);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Scale"), &scale[0], 0.1f)) {
        mTransform.SetScale(scale);
    }
    mMeshRenderer.mMaterial->OnImGuiRender();
}