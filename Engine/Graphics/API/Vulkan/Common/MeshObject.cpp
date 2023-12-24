//
// Created by Remus on 17/12/2023.
//

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glog/logging.h"
#include "MeshObject.h"

#include "imgui.h"
#include "Base/Common/Material.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Data/Mesh.h"
#include "Vulkan/GraphicsPipeline.h"

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

void MeshObject::CreateObject(GraphicsPipeline &aBoundGraphicsPipeline,
                              const char *aName) {
    mName = aName;
    LOG(INFO) << "Creating Object: " << mName;

    CreateRenderer(aBoundGraphicsPipeline);
}

void MeshObject::CreateRenderer(
    GraphicsPipeline &aBoundGraphicsPipeline) {
    // Load Shaders
    mGraphicsPipeline = &aBoundGraphicsPipeline;

    mGraphicsPipeline->AddRenderer(this);
    mMesh = new Mesh();
    mMaterial = new Material();


    // Binds Camera Uniform Buffer
    mMaterial->AddBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    //// Binds Lighting Uniform Buffer
    mMaterial->AddBinding(1, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);


    mMaterial->Create(mMaterial, mName);
    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        mMaterial->SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mCameraBuffer, 0, 0);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        mMaterial->SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mLightingBuffer, 1, 0);
}

void MeshObject::DestroyRenderer() {
    delete mMesh;
    mMaterial->Destroy();
    delete mMaterial;
}

void MeshObject::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame) {
    mPushConstants.model = mTransform.GetCombinedMatrix();
    Renderer::Render(aCommandBuffer, aImageIndex, aCurrentFrame);
}

void MeshObject::OnImGuiRender() {
    if (ImGui::CollapsingHeader(mName)) {
        ImGui::Indent();
        glm::vec3 pos = mTransform.Position();
        glm::vec3 rot = mTransform.Euler();
        glm::vec3 scale = mTransform.Scale();

        const std::string hash = std::string("##") + mName;
        if (ImGui::InputFloat3(("Position" + hash).c_str(), &pos[0])) {
            mTransform.SetPosition(pos);
        }
        if (ImGui::InputFloat3(("Rotation" + hash).c_str(), &rot[0])) {
            mTransform.SetRotation(rot);
        }
        if (ImGui::InputFloat3(("Scale" + hash).c_str(), &scale[0])) {
            mTransform.SetScale(scale);
        }
        ImGui::Unindent();
    }
}

void Renderer::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                      uint32_t aCurrentFrame) {
    mMesh->Bind(aCommandBuffer);

    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);

    vkCmdDraw(aCommandBuffer, mMesh->GetVertices().size(), 1, 0, 0);
}
