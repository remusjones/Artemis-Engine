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
}

void MeshObject::DestroyRenderer() {
    delete mMesh;
}

void MeshObject::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame, glm::mat4 aCameraViewMatrix) {
    mPushConstants.model = aCameraViewMatrix * mTransform.mTransformationMatrix;
    Renderer::Render(aCommandBuffer, aImageIndex, aCurrentFrame,
                     aCameraViewMatrix);
}

void Renderer::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                      uint32_t aCurrentFrame, glm::mat4 aProjectionMatrix) {
    mMesh->Bind(aCommandBuffer);

    vkCmdPushConstants(aCommandBuffer, mGraphicsPipeline->mPipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(PushConstants), &mPushConstants);

    vkCmdDraw(aCommandBuffer, mMesh->GetVertices().size(), 1, 0, 0);
}
