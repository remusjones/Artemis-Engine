//
// Created by Remus on 6/11/2021.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef INC_3DENGINE_SHADERMODULE_H
#define INC_3DENGINE_SHADERMODULE_H

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

    void Initialize(VkDevice& logicalDevice,
                    VulkanSwapChain* remSwapChain,
                    VkPhysicalDevice& physicalDevice,
                    VkQueue& graphicsQueue,
                    VkQueue& presentQueue
                    );

    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    VkResult CreateVertexBuffer(const std::vector<Vertex>& vertices);

    VkResult CreateIndexBuffer();

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer& buffer,
                      VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VulkanMaterial* LoadShader(const std::string& shaderName);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void CreatePipelineLayout();
    void CreateCommandPool(QueueFamilyIndices& queueFamilyIndices);
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void DrawFrame();
    void CreateSyncObjects();

    void CleanupBuffers();
    void Cleanup();
    void DestroyShader(VulkanMaterial* shaderComponent);

    std::vector<VkFence> m_inFlightFences;
    bool m_framebufferResized = false;
    std::vector<VkFence> m_imagesInFlight;


    // TODO: House the vert buffer somewhere else
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
    VkBuffer m_indexBuffer;
    VkDeviceMemory indexBufferMemory;
private:

    void CleanupOldSyncObjects();
    VulkanSwapChain* m_remSwapChain;
    std::vector<VulkanMaterial*> m_loadedMaterials{};
    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    bool m_hasCreatedPipeline = false;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    std::vector<VkSemaphore>  m_imageAvailableSemaphores;
    std::vector<VkSemaphore>  m_renderFinishedSemaphores;

    std::vector<VkFence> m_inFlightFencesToDestroy;
    std::vector<VkSemaphore>  m_imageAvailableSemaphoresToDestroy;
    std::vector<VkSemaphore>  m_renderFinishedSemaphoresToDestroy;


    size_t m_currentFrame = 0;

    //TODO: move these into a specific object
    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

};


#endif //INC_3DENGINE_SHADERMODULE_H
