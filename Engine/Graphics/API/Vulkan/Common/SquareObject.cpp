//
// Created by Remus on 17/12/2023.
//

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SquareObject.h"
#include "API/Base/Common/Material.h"
#include "GraphicsPipeline.h"
#include "API/Base/Common/Buffer.h"
#include "VulkanGraphicsImpl.h"
#include "API/Base/Common/UniformBuffer.h"

void SquareObject::CreateObject(GraphicsPipeline& aBoundGraphicsPipeline) {

    // Load Shaders
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddShader("/Shaders/UniformBuffer_v.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    mGraphicsPipeline->AddShader("/Shaders/VertexLit_f.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
    mGraphicsPipeline->AddRenderer(this);
    // Create Vertices & Indices (pretend mesh)
    mVertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 0.5f, 1.0f}}
    };

    mIndices = {
            0, 1, 2, 2, 3, 0
    };


    // Create and Bind Material Information
    // TODO: Utilize when making push-constants
    //MaterialBase* materialBase = new MaterialBase();
    //materialBase->AddBinding(0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
    //                         VK_SHADER_STAGE_ALL_GRAPHICS); // Unused atm
    //materialBase->AddBinding(1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_SHADER_STAGE_ALL);
    //materialBase->AddBinding(2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
    //materialBase->Create();
    //mMaterial = new Material();
    //mMaterial->Create(materialBase, "mSquare");

    // Create Buffers
    mVertexBuffer = new Buffer(mVertices.data(), sizeof(mVertices[0]) * mVertices.size(),
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    mIndexBuffer = new Buffer(mIndices.data(), sizeof(mIndices[0]) * mIndices.size(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    mUniformBuffer = new UniformBuffer();


}

void SquareObject::RotateObject(uint32_t aCurrentFrame) {

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspectiveRH(glm::radians(45.0f), (float)gGraphics->mSwapChain->mSwapChainExtent.width / (float)
    gGraphics->mSwapChain->mSwapChainExtent.height, 0.1f,
                              10.0f);

    memcpy(mUniformBuffer->uniformBuffersMapped[aCurrentFrame], &ubo, sizeof(ubo));
}

void SquareObject::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex, uint32_t aCurrentFrame) {

    VkBuffer vertexBuffers[] = {mVertexBuffer->mBuffer};
    VkDeviceSize offsets[] = {0};

    RotateObject(aCurrentFrame);
    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mIndexBuffer->mBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);

}

void SquareObject::Destroy() {

    if (mVertexBuffer) {
        mVertexBuffer->Destroy();
        delete mVertexBuffer;
    }

    if (mIndexBuffer) {
        mIndexBuffer->Destroy();
        delete mIndexBuffer;
    }

    if (mUniformBuffer)
    {
        mUniformBuffer->Destroy();
        delete mUniformBuffer;
    }
}

