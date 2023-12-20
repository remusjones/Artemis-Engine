//
// Created by Remus on 17/12/2023.
//

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glog/logging.h"
#include "SquareObject.h"
#include "Base/Common/Material.h"
#include "Base/Common/Buffers/UniformBuffer.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"
#include "VulkanGraphicsImpl.h"
#include "Vulkan/GraphicsPipeline.h"

void SquareObject::CreateObject(GraphicsPipeline &aBoundGraphicsPipeline,
                                const char *aName) {
    mName = aName;
    LOG(INFO) << "Creating Object: " << mName;

    InitializeRenderer(aBoundGraphicsPipeline);
}

void SquareObject::InitializeRenderer(
    GraphicsPipeline &aBoundGraphicsPipeline) {
    // Load Shaders
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddShader("/Shaders/3DObject_v.spv",
                                 VK_SHADER_STAGE_VERTEX_BIT);
    mGraphicsPipeline->AddShader("/Shaders/VertexLit_f.spv",
                                 VK_SHADER_STAGE_FRAGMENT_BIT);
    mGraphicsPipeline->AddRenderer(this);


    mVertices.resize(4);

    //vertex positions
    mVertices[0].mPosition = {-0.5f, -0.5f, 0.0f};
    mVertices[1].mPosition = {0.5f, -0.5f, 0.0f};
    mVertices[2].mPosition = {0.5f, 0.5f, 0.0f};
    mVertices[3].mPosition = {-0.5f, 0.5f, 0.0f};

    //vertex colors, all green
    mVertices[0].mColor = {0.f, 1.f, 0.0f}; //pure green
    mVertices[1].mColor = {0.f, 1.f, 0.0f}; //pure green
    mVertices[2].mColor = {0.f, 1.f, 0.0f}; //pure green
    mVertices[3].mColor = {0.f, 1.f, 0.0f}; //pure green

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
    mVertexBuffer = new AllocatedVertexBuffer(mVertices, mIndices);
    mUniformBuffer = new UniformBuffer();
}

void SquareObject::RotateObject(uint32_t aCurrentFrame) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
        currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(15.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspectiveRH(glm::radians(45.0f),
                                  (float) gGraphics->mSwapChain->
                                  mSwapChainExtent.width / (float)
                                  gGraphics->mSwapChain->mSwapChainExtent.
                                  height, 0.1f,
                                  10.0f);

    memcpy(mUniformBuffer->uniformBuffersMapped[aCurrentFrame], &ubo,
           sizeof(ubo));
}

void SquareObject::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                          uint32_t aCurrentFrame) {
    RotateObject(aCurrentFrame);

    Renderer::Render(aCommandBuffer, aImageIndex, aCurrentFrame);
}

void SquareObject::Destroy() {
    delete mVertexBuffer;
    delete mUniformBuffer;
}


Renderer::Renderer(): mUniformBuffer(nullptr), mVertexBuffer(nullptr),
                      mMaterial(nullptr),
                      mGraphicsPipeline(nullptr) {
}

void Renderer::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                      uint32_t aCurrentFrame) {
    VkBuffer vertexBuffers[] = {mVertexBuffer->mVerticesBuffer->mBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mVertexBuffer->mIndicesBuffer->mBuffer,
                         0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1,
                     0, 0, 0);
}
