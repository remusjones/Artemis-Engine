//
// Created by Remus on 24/01/2024.
//

#include "LineRenderer.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Data/Vertex.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void LineRenderer::SetLinePositions(const std::vector<glm::vec3> &aPositions) {

    if (mLinePositions.size() == aPositions.size()) {
        for (int i = 0; i < aPositions.size(); i++)
            mLinePositions[i] = Position(aPositions[i]);
        return;
    }

    mLinePositions = std::vector<Position>(aPositions.size());
    for (int i = 0; i < aPositions.size(); i++)
        mLinePositions[i] = Position(aPositions[i]);

    if (mAllocatedPositions) {
        mAllocatedPositions->Destroy();
        delete mAllocatedPositions;
    }

    mAllocatedPositions = new AllocatedBuffer(mLinePositions.data(),
                                              sizeof(Position) * mLinePositions.size(),
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void LineRenderer::DestroyRenderer() {
    Renderer::DestroyRenderer();
    if (mAllocatedPositions) {
        mAllocatedPositions->Destroy();
        delete mAllocatedPositions;
    }
}

void LineRenderer::BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) {
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddRenderer(this);
}

void LineRenderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    if (mAllocatedPositions) {

        void *data;
        vmaMapMemory(gGraphics->mAllocator, mAllocatedPositions->mAllocation, &data);
        memcpy(data, mLinePositions.data(), sizeof(Position) * mLinePositions.size());
        vmaUnmapMemory(gGraphics->mAllocator, mAllocatedPositions->mAllocation);


        const VkBuffer vertexBuffers[] = {mAllocatedPositions->mBuffer};
        const VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);

        mPushConstants.model = mTransform->GetWorldMatrix();
        vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineConfig.pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstants), &mPushConstants);

        vkCmdDraw(aCommandBuffer, mLinePositions.size(), 1, 0, 0);//
    }
}
