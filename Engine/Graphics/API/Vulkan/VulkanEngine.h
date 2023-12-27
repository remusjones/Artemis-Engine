//
// Created by Remus on 6/11/2021.
//
#pragma once

#include <functional>
#include <vector>
#include "VulkanSwapChain.h"
#include "Base/Common/Data/Vertex.h"
#include "Base/Common/Data/FrameData.h"

class Material;
class Scene;
class GraphicsPipeline;

struct UploadContext {
    VkFence mUploadFence;
    VkCommandPool mCommandPool;
    VkCommandBuffer mCommandBuffer;
};

class VulkanEngine {
public:
    void Initialize(VkDevice &aLogicalDevice,
                    VulkanSwapChain *aSwapChain,
                    VkPhysicalDevice &aPhysicalDevice,
                    VkQueue &aGraphicsQueue,
                    VkQueue &aPresentQueue
    );

    const FrameData &GetCurrentFrame() { return mFrameData[mCurrentFrame]; }
    const FrameData &GetFrame(int32_t aIndex) { return mFrameData[aIndex]; }

    void SubmitBufferCommand(std::function<void(VkCommandBuffer cmd)> &&function) const;

    void QueueFrameBufferRebuild() { mRebuildFrameBuffer = true; }

    void CreateUploadContext();

    void CreateCommandPool();

    void CreateCommandBuffers();

    void DestroyCommandPool();

    void CreateDescriptorPool();


    void DrawFrame(Scene &aActiveScene);

    void CreateSyncObjects();

    void Cleanup();

    AllocatedBuffer CreateBuffer(size_t aAllocSize, VkBufferUsageFlags aUsage, VmaMemoryUsage vmaMemoryUsage);

private:
    void CleanupOldSyncObjects();

public:
    std::vector<VkFence> mImagesInFlight;
    VulkanSwapChain *mSwapChain;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkDescriptorPool mDescriptorPool;
    UploadContext mUploadContext;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

private:
    bool mRebuildFrameBuffer = false;
    // Cached Variables for layouts
    VkDevice mLogicalDevice;
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mDeviceProperties;

    std::vector<FrameData> mFrameData;

    std::vector<VkFence> mInFlightFencesToDestroy;
    std::vector<VkSemaphore> mImageAvailableSemaphoresToDestroy;
    std::vector<VkSemaphore> mRenderFinishedSemaphoresToDestroy;

    size_t mCurrentFrame = 0;
};
