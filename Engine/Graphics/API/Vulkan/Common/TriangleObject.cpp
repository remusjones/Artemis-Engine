//
// Created by Remus on 17/12/2023.
//

#include "TriangleObject.h"
#include "API/Base/Common/Material.h"
#include "GraphicsPipeline.h"

void TriangleObject::CreateObject() {

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


    MaterialBase* materialBase = new MaterialBase();
    materialBase->AddBinding(0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR,
                             VK_SHADER_STAGE_ALL_GRAPHICS); // Unused atm

    //materialBase->AddBinding(1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,VK_SHADER_STAGE_ALL);
    //materialBase->AddBinding(2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
    materialBase->Create();


    mMaterial = new Material();
    mMaterial->Create(materialBase, "Triangle");


    // Create Buffers

}

void TriangleObject::Render(VkCommandBuffer aCommandBuffer) {

    vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
}

void TriangleObject::Destroy() {
    if (mGraphicsPipeline != nullptr)
    {
        mGraphicsPipeline->Destroy();
        delete mGraphicsPipeline;
    }
}
