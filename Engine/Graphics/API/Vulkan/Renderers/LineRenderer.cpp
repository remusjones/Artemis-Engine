//
// Created by Remus on 24/01/2024.
//

#include "LineRenderer.h"
#include "Base/Common/Color.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Data/Vertex.h"
#include "Objects/Camera.h"
#include "Vulkan/Systems/GraphicsPipeline.h"


void LineRenderer::SetLinePositions(const std::vector<glm::vec3> &aPositions, const LineRenderMode aMode) {
    const glm::vec3 color = Color::White().RGB();

    mLineRenderMode = aMode;

    if (mLinePositions.size() == aPositions.size()) {
        for (int i = 0; i < aPositions.size(); i++) {
            mLinePositions[i] = {aPositions[i], {}, color};
        }
        return;
    }

    mLinePositions = std::vector<Vertex>(aPositions.size());
    for (int i = 0; i < aPositions.size(); i++) {
        mLinePositions[i] = {aPositions[i], {}, color};
    }

    if (mAllocatedPositions) {
        mAllocatedPositions->Destroy();
        delete mAllocatedPositions;
    }

    mAllocatedPositions = new AllocatedBuffer(mLinePositions.data(),
                                              sizeof(Vertex) * mLinePositions.size(),
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void LineRenderer::SetLinePositions(const std::vector<glm::vec3> &aPositions, const std::vector<Color> &aColors,
const LineRenderMode aMode) {

    mLineRenderMode = aMode;
    if (mLinePositions.size() == aPositions.size()) {
        for (int i = 0; i < aPositions.size(); i++) {
            mLinePositions[i] = {aPositions[i], {}, aColors[i].RGB()};
        }
        return;
    }

    mLinePositions = std::vector<Vertex>(aPositions.size());
    for (int i = 0; i < aPositions.size(); i++){
            mLinePositions[i] = {aPositions[i], {}, aColors[i].RGB()};
    }

    if (mAllocatedPositions) {
        mAllocatedPositions->Destroy();
        delete mAllocatedPositions;
    }

    mAllocatedPositions = new AllocatedBuffer(mLinePositions.data(),
                                              sizeof(Vertex) * mLinePositions.size(),
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
        memcpy(data, mLinePositions.data(), sizeof(mLinePositions[0]) * mLinePositions.size());
        vmaUnmapMemory(gGraphics->mAllocator, mAllocatedPositions->mAllocation);

        const VkBuffer vertexBuffers[] = {mAllocatedPositions->mBuffer};
        const VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);

        mPushConstants.model = mTransform->GetWorldMatrix();
        vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineConfig.pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(PushConstants), &mPushConstants);

        switch (mLineRenderMode) {
            case LINES_CONTINUOUS:
                vkCmdDraw(aCommandBuffer, mLinePositions.size(), 1, 0, 0);
                break;
            case LINES_SEGMENTED:
                for (int i = 0; i < mLinePositions.size() - 1; i += 2) {
                    vkCmdDraw(aCommandBuffer, 2, 1, i, 0);
                }
                break;
            default: break;
        }
    }
}
