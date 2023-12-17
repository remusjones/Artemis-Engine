//
// Created by Remus on 6/11/2021.
//
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <map>
#include "VulkanSystemStructs.h"
#include "VulkanSwapChain.h"
#include "Common/VulkanMaterial.h"
#include "API/Base/Common/Data/Vertex.h"
#include "API/Base/Common/Buffer.h"

class GraphicsPipeline;

class VulkanPipelineManager
{
public:

    void Initialize(VkDevice& aLogicalDevice,
                    VulkanSwapChain* aSwapChain,
                    VkPhysicalDevice& aPhysicalDevice,
                    VkQueue& aGraphicsQueue,
                    VkQueue& aPresentQueue
                    );

    void CreateCommandPool(const QueueFamilyIndices& aQueueFamilyIndices);
    void CreateCommandBuffers();
    void AddGraphicsPipeline(GraphicsPipeline* aGraphicsPipeline);
    void RecordCommandBuffer(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex);
    void DrawFrame();
    void CreateSyncObjects();
    void Cleanup();

private:
    void CleanupOldSyncObjects();

public:

    bool mFramebufferResized = false;
    std::vector<GraphicsPipeline*> mGraphicsPipelines;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;

    VulkanSwapChain* mSwapChain;
    std::vector<VulkanMaterial*> mLoadedMaterials{};

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkCommandPool mCommandPool;

private:
    // Cached Variables for layouts
    VkDevice mLogicalDevice;
    VkPhysicalDevice mPhysicalDevice;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicPipeline;
    VkPhysicalDeviceProperties mDeviceProperties;


    std::vector<VkCommandBuffer> mCommandBuffers;
    // Semaphores and Fences
    std::vector<VkSemaphore>  mImageAvailableSemaphores;
    std::vector<VkSemaphore>  mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFencesToDestroy;
    std::vector<VkSemaphore>  mImageAvailableSemaphoresToDestroy;
    std::vector<VkSemaphore>  mRenderFinishedSemaphoresToDestroy;

    size_t mCurrentFrame = 0;

    //TODO: move these into a specific object
    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

};