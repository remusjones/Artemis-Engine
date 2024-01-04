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
#include "Vulkan/GraphicsPipeline.h"
#include <glm/gtc/type_ptr.hpp>

void MeshObject::Construct() {
    Super::Construct();
}

void MeshObject::Tick(float aDeltaTime) {
    Super::Tick(aDeltaTime);
}

void MeshObject::Cleanup() {
    DestroyRenderer();
    Super::Cleanup();
}

void MeshObject::CreateObject(
    GraphicsPipeline &aBoundGraphicsPipeline,
    Material &aMaterial,
    const char *aName) {
    mName = aName;
    Logger::Log(spdlog::level::info, (std::string("Creating object ") + mName).c_str());
    mMaterial = &aMaterial;
    CreateRenderer(aBoundGraphicsPipeline);
}

void MeshObject::CreateRenderer(
    GraphicsPipeline &aBoundGraphicsPipeline) {
    // Load Shaders
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddRenderer(this);

    mMesh = new Mesh();
}

void MeshObject::DestroyRenderer() {
    delete mMesh;
    mMaterial->Destroy();
}

void MeshObject::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    mPushConstants.model = mTransform.GetWorldMatrix();
    Renderer::Render(aCommandBuffer, aScene);
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
    mMaterial->OnImGuiRender();

}

void Renderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    void *data;
    vmaMapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation, &data);
    memcpy(data, &mMaterial->mMaterialProperties, sizeof(MaterialProperties));
    vmaUnmapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation);

    mMesh->Bind(aCommandBuffer);
    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);
    vkCmdDraw(aCommandBuffer, mMesh->GetVertices().size(), 1, 0, 0);
}
