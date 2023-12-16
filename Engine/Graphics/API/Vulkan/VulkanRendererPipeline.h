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

class VulkanRendererPipeline
{
public:

    void Initialize(VkDevice& aLogicalDevice,
                    VulkanSwapChain* aSwapChain,
                    VkPhysicalDevice& aPhysicalDevice,
                    VkQueue& aGraphicsQueue,
                    VkQueue& aPresentQueue
                    );

    VkShaderModule CreateShaderModule(const std::vector<char>& aCode);
    VulkanMaterial* LoadShader(const std::string& aShaderName);
    VkResult CreateVertexBuffer(const std::vector<Vertex>& aVertices);
    VkResult CreateIndexBuffer();

    void CreateBuffer(VkDeviceSize aSize, VkBufferUsageFlags aUsage, VkMemoryPropertyFlags aProperties,
                      VkBuffer& aBuffer,
                      VkDeviceMemory& aBufferMemory);

    void CopyBuffer(VkBuffer aSrcBuffer, VkBuffer aDstBuffer, VkDeviceSize aSize);

    uint32_t FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties);
    void CreatePipelineLayout();
    void CreateCommandPool(const QueueFamilyIndices& aQueueFamilyIndices);
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex);
    void DrawFrame();
    void CreateSyncObjects();
    void CleanupBuffers();
    void Cleanup();

private:
    void CleanupOldSyncObjects();

public:
    std::vector<VkFence> mInFlightFences;
    bool mFramebufferResized = false;
    std::vector<VkFence> mImagesInFlight;

    // TODO: House the vert buffer somewhere else
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    VulkanSwapChain* mSwapChain;
    std::vector<VulkanMaterial*> mLoadedMaterials{};

private:
    // Cached Variables for layouts
    VkDevice mLogicalDevice;
    VkPhysicalDevice mPhysicalDevice;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    VkCommandPool mCommandPool;


    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

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