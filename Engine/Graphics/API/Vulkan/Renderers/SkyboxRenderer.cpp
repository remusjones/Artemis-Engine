//
// Created by Remus on 9/01/2024.
//

#include "SkyboxRenderer.h"

#include "Base/Common/Data/PrimativeMesh.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SkyboxRenderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    mMesh->Bind(aCommandBuffer);
    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineConfig.pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);

    vkCmdDraw(aCommandBuffer, mMesh->GetVerticesSize(), 1, 0, 0);
    mPushConstants.model = mTransform->GetWorldMatrix();
}

void SkyboxRenderer::BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) {
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddRenderer(this);
}

void SkyboxRenderer::DestroyRenderer() {
    delete mMesh;
    Renderer::DestroyRenderer();
}

void SkyboxRenderer::LoadMesh(const char *aPath, const char *aMtlPath) {
    mMesh = new PrimativeMesh();
    mMesh->LoadFromObject(aPath, aMtlPath);
}
