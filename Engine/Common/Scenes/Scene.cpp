//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"

#include "VulkanGraphicsImpl.h"
#include "glog/logging.h"
#include "Objects/Camera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"


void Scene::Construct(const char *aSceneName) {
    for (const auto obj: mObjects) {
        obj->Construct();
    }
    assert(mActiveCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame) {

    FrameData currentFrame = gGraphics->mRenderPipelineManager.GetCurrentFrame();
    GPUCameraData camData;
    camData.mPerspectiveMatrix = mActiveCamera->GetPerspectiveMatrix();
    camData.mViewMatrix = mActiveCamera->GetViewMatrix();
    camData.mViewProjectionMatrix = mActiveCamera->GetViewProjectionMatrix();

    for (const auto obj: mGraphicsPipelines) {

        vkCmdBindDescriptorSets(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, obj->mPipelineLayout, 0,
            1, &currentFrame.mGlobalDescriptor, 0, nullptr);
        void* data;
        vmaMapMemory(gGraphics->mAllocator, currentFrame.mCameraBuffer.mAllocation, &data);
        memcpy(data, &camData, sizeof(GPUCameraData));
        vmaUnmapMemory(gGraphics->mAllocator, currentFrame.mCameraBuffer.mAllocation);

        obj->Draw(aCommandBuffer, aImageIndex, aCurrentFrame,
                  mActiveCamera->GetViewProjectionMatrix());
    }
}

void Scene::Tick(float aDeltaTime) {
    for (const auto obj: mObjects) {
        obj->Tick(aDeltaTime);
    }
}

void Scene::Cleanup() {
    LOG(INFO) << "Cleaning up scene " << mSceneName;
    for (const auto obj: mObjects) {
        obj->Cleanup();
        delete obj;
    }

    delete mActiveCamera;
    for (const auto pipeline: mGraphicsPipelines) {
        pipeline->Destroy();
        delete pipeline;
    }
}

void Scene::AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline) {
    LOG(INFO) << "Creating Graphics Pipeline: " << aGraphicsPipeline->
            mPipelineName;
    mGraphicsPipelines.push_back(aGraphicsPipeline);
    aGraphicsPipeline->Create();
}
