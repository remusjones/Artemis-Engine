//
// Created by Remus on 9/01/2024.
//

#include "MeshRenderer.h"

#include "Vulkan/Systems/GraphicsPipeline.h"

void MeshRenderer::LoadMesh(const char *aPath, const char *aMtlPath) {
    mMesh = new Mesh();
    mMesh->LoadFromObject(aPath, aMtlPath);
}

void MeshRenderer::DestroyRenderer() {
    delete mMesh;
    Renderer::DestroyRenderer();
}

void MeshRenderer::BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) {
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddRenderer(this);
}

void MeshRenderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    mMesh->Bind(aCommandBuffer);
    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineConfig.pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);
    vkCmdDraw(aCommandBuffer, mMesh->GetVerticesSize(), 1, 0, 0);
    mPushConstants.model = mTransform->GetWorldMatrix();
    Renderer::Render(aCommandBuffer, aScene);
}
