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
#include "SystemStructs.h"
#include "Systems/Rendering/RemMaterial.h"
#include "RemSwapChain.h"

class RemPipeline
{
public:

    void Initialize(VkDevice& logicalDevice,
                    RemSwapChain* remSwapChain,
                    VkPhysicalDevice& physicalDevice,
                    VkQueue& graphicsQueue,
                    VkQueue& presentQueue
                    );

    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    VkResult CreateVertexBuffer(const std::vector<Vertex>& vertices,
                                VkBuffer& vertexBuffer,
                                VkDeviceMemory& allocatedMemory);

    RemMaterial* LoadShader(const std::string& shaderName);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void CreatePipelineLayout();
    void CreateCommandPool(QueueFamilyIndices& queueFamilyIndices);
    void CreateCommandBuffer();
    void DrawFrame();
    void CreateSyncObjects();

    void CleanupVertexBuffer();
    void Cleanup();
    void DestroyShader(RemMaterial* shaderComponent);

    std::vector<VkFence> m_inFlightFences;
    bool m_framebufferResized = false;
    std::vector<VkFence> m_imagesInFlight;


    // TODO: House the vert buffer somewhere else
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexBufferMemory;
private:

    void CleanupOldSyncObjects();
    RemSwapChain* m_remSwapChain;
    std::vector<RemMaterial*> m_loadedMaterials{};
    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers{};
    bool m_hasCreatedPipeline = false;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    std::vector<VkSemaphore>  m_imageAvailableSemaphores;
    std::vector<VkSemaphore>  m_renderFinishedSemaphores;


    std::vector<VkFence> m_inFlightFencesToDestroy;
    std::vector<VkSemaphore>  m_imageAvailableSemaphoresToDestroy;
    std::vector<VkSemaphore>  m_renderFinishedSemaphoresToDestroy;


    size_t m_currentFrame = 0;



    const int MAX_FRAMES_IN_FLIGHT = 2;

};


#endif //INC_3DENGINE_SHADERMODULE_H
