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
#include "Scenes/Scene.h"
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

    // Binds Lighting Uniform Buffer
    mMaterial->AddBinding(1, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    // Material Properties
    mMaterial->AddBinding(2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    mMaterial->Create(mMaterial, mName);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        mMaterial->SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mCameraBuffer, 0, 0);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        mMaterial->SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mLightingBuffer, 1, 0);

    mMaterial->CreateProperties(2, MaterialProperties());
}

void MeshObject::DestroyRenderer() {
    delete mMesh;
    mMaterial->Destroy();
    delete mMaterial;
}

void MeshObject::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    mPushConstants.model = mTransform.GetCombinedMatrix();
    Renderer::Render(aCommandBuffer, aScene);
}

void MeshObject::OnImGuiRender() {
    if (ImGui::CollapsingHeader(mName)) {
        ImGui::Indent();

        ImGui::SeparatorText("Transform");
        glm::vec3 pos = mTransform.Position();
        glm::vec3 rot = mTransform.Euler();
        glm::vec3 scale = mTransform.Scale();

        if (ImGui::DragFloat3(GetHashedName("Position"), &pos[0], 0.1f)) {
            mTransform.SetPosition(pos);
        }
        if (ImGui::DragFloat3(GetHashedName("Rotation"), &rot[0], 0.1f)) {
            mTransform.SetRotation(rot);
        }
        if (ImGui::DragFloat3(GetHashedName("Scale"), &scale[0], 0.1f)) {
            mTransform.SetScale(scale);
        }

        ImGui::SeparatorText("Material");
        ImGui::ColorEdit4(GetHashedName("Color"), &mMaterial->mMaterialProperties.mColor[0]);
        if (ImGui::DragFloat(GetHashedName("Shininess"),
            &mMaterial->mMaterialProperties.mShininess, 0.1f)) {
        }
        if (ImGui::DragFloat(GetHashedName("Specular"),
            &mMaterial->mMaterialProperties.mSpecularStrength, 0.1f)) {
        }
        ImGui::Unindent();
    }
}

void Renderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    FrameData currentFrame = gGraphics->mVulkanEngine.GetCurrentFrame();
    GPUCameraData camData;
    camData.mPerspectiveMatrix = aScene.mActiveCamera->GetPerspectiveMatrix();
    camData.mViewMatrix = aScene.mActiveCamera->GetViewMatrix();
    camData.mViewProjectionMatrix = aScene.mActiveCamera->GetViewProjectionMatrix();


    void *data;

    vmaMapMemory(gGraphics->mAllocator, currentFrame.mCameraBuffer.mAllocation, &data);
    memcpy(data, &camData, sizeof(GPUCameraData));
    vmaUnmapMemory(gGraphics->mAllocator, currentFrame.mCameraBuffer.mAllocation);

    vmaMapMemory(gGraphics->mAllocator, currentFrame.mLightingBuffer.mAllocation, &data);
    memcpy(data, &aScene.mSceneData, sizeof(GPUSceneData));
    vmaUnmapMemory(gGraphics->mAllocator, currentFrame.mLightingBuffer.mAllocation);

    vmaMapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation, &data);
    memcpy(data, &mMaterial->mMaterialProperties, sizeof(MaterialProperties));
    vmaUnmapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation);

    mMesh->Bind(aCommandBuffer);
    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);

    vkCmdDraw(aCommandBuffer, mMesh->GetVertices().size(), 1, 0, 0);
}
