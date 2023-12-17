//
// Created by Remus on 17/12/2023.
//

#include "TriangleObject.h"
#include "API/Base/Common/Material.h"
#include "GraphicsPipeline.h"
#include "API/Base/Common/Buffer.h"

void TriangleObject::CreateObject() {

    // Load Shaders
    mGraphicsPipeline = new GraphicsPipeline();
    mGraphicsPipeline->AddShader("/Shaders/triangleInput_v.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
    mGraphicsPipeline->AddShader("/Shaders/triangleInput_f.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);

    // Create Vertices & Indices (pretend mesh)
    mVertices = {
        {{-0.5f, -0.5f}, {.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}},
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
    //mMaterial->Create(materialBase, "mTriangle");

    // Create Buffers
    mVertexBuffer = new Buffer(mVertices.data(), sizeof(mVertices[0]) * mVertices.size(),
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    mIndexBuffer = new Buffer(mIndices.data(), sizeof(mIndices[0]) * mIndices.size(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

}

void TriangleObject::Render(VkCommandBuffer aCommandBuffer) {

    vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
}

void TriangleObject::Destroy() {

    if (mGraphicsPipeline != nullptr) {
        mGraphicsPipeline->Destroy();
        delete mGraphicsPipeline;
    }

    if (mVertexBuffer) {
        mVertexBuffer->Cleanup();
        delete mVertexBuffer;
    }

    if (mIndexBuffer) {
        mIndexBuffer->Cleanup();
        delete mIndexBuffer;
    }
}