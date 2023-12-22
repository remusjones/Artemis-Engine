//
// Created by Remus on 17/12/2023.
//

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glog/logging.h"
#include "MeshObject.h"
#include "Base/Common/Material.h"
#include "Base/Common/Buffers/PushConstants.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Data/Mesh.h"
#include "File Management/FileManagement.h"
#include "Vulkan/GraphicsPipeline.h"

void MeshObject::CreateObject(GraphicsPipeline &aBoundGraphicsPipeline,
                                const char *aName) {
    mName = aName;
    LOG(INFO) << "Creating Object: " << mName;

    InitializeRenderer(aBoundGraphicsPipeline);
}

void MeshObject::InitializeRenderer(
    GraphicsPipeline &aBoundGraphicsPipeline) {
    // Load Shaders
    mGraphicsPipeline = &aBoundGraphicsPipeline;
    mGraphicsPipeline->AddShader("/Shaders/3DObject_v.spv",
                                 VK_SHADER_STAGE_VERTEX_BIT);
    mGraphicsPipeline->AddShader("/Shaders/VertexLit_f.spv",
                                 VK_SHADER_STAGE_FRAGMENT_BIT);
    mGraphicsPipeline->AddRenderer(this);
    mMesh = new Mesh();
    mMesh->LoadFromObject((FileManagement::GetCurrentDirectory() +
    std::string("/../Models/monkey_smooth.obj")).c_str());

}

void MeshObject::RotateObject(uint32_t aCurrentFrame) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
        currentTime - startTime).count();



    mPushConstants.model = glm::rotate(glm::mat4(1.0f),
                                       time * glm::radians(15.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f));
    mPushConstants.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f));
    mPushConstants.proj = glm::perspectiveRH(glm::radians(45.0f),
                                             (float) gGraphics->mSwapChain->
                                             mSwapChainExtent.width / (float)
                                             gGraphics->mSwapChain->
                                             mSwapChainExtent.
                                             height, 0.1f,
                                             100.0f);
}

void MeshObject::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                          uint32_t aCurrentFrame) {
    RotateObject(aCurrentFrame);

    Renderer::Render(aCommandBuffer, aImageIndex, aCurrentFrame);
}


Renderer::Renderer() {
}

Renderer::~Renderer() {
    delete mMesh;
}

void Renderer::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                      uint32_t aCurrentFrame) {

    mMesh->Bind(aCommandBuffer);

    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);

    //vkCmdDraw(aCommandBuffer, mVertices.size(), 1, 0, 0);
    //vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mMesh->GetIndices
    //().size()),1,0, 0, 0);
    vkCmdDraw(aCommandBuffer, mMesh->GetVertices().size(), 1, 0, 0);
}
