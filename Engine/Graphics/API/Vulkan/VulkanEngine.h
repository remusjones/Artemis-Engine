//
// Created by Remus on 6/11/2021.
//
#pragma once

#include <vector>
#include "VulkanSwapChain.h"
#include "Base/Common/Data/Vertex.h"
#include "Base/Common/Buffers/Buffer.h"

class Material;
class Scene;
class GraphicsPipeline;

class VulkanEngine
{
public:

    void Initialize(VkDevice& aLogicalDevice,
                    VulkanSwapChain* aSwapChain,
                    VkPhysicalDevice& aPhysicalDevice,
                    VkQueue& aGraphicsQueue,
                    VkQueue& aPresentQueue
                    );

    void CreateCommandPool();
    void CreateCommandBuffers();

    void DrawFrame(Scene& aActiveScene);
    void CreateSyncObjects();
    void Cleanup();

private:
    void CleanupOldSyncObjects();

public:

    bool mFramebufferResized = false;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;

    VulkanSwapChain* mSwapChain;
    std::vector<Material*> mLoadedMaterials{};

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkCommandPool mCommandPool;
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
private:

    // Cached Variables for layouts
    VkDevice mLogicalDevice;
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mDeviceProperties;

    std::vector<VkCommandBuffer> mCommandBuffers;
    // Semaphores and Fences
    std::vector<VkSemaphore>  mImageAvailableSemaphores;
    std::vector<VkSemaphore>  mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFencesToDestroy;
    std::vector<VkSemaphore>  mImageAvailableSemaphoresToDestroy;
    std::vector<VkSemaphore>  mRenderFinishedSemaphoresToDestroy;

    size_t mCurrentFrame = 0;
};

