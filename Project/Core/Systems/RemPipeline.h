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
    VkBuffer CreateVertexBuffer(const std::vector<Vertex>& verticies);

    VkResult LoadShader(const std::string& shaderName, RemMaterial&
    shaderComponent);

    VkResult VertexBuffer(const RemMaterial& shaderComponent,
                          const std::vector<Vertex>& verticies);
    void CreatePipelineLayout();
    void CreateCommandPool(QueueFamilyIndices& queueFamilyIndices);
    void CreateCommandBuffer();
    void DrawFrame();
    void CreateSyncObjects();

    void Cleanup();
    void DestroyShader(RemMaterial* shaderComponent);

    std::vector<VkFence> m_inFlightFences;
    bool m_framebufferResized = false;
    std::vector<VkFence> m_imagesInFlight;
private:
    void CleanupOldSyncObjects();

    RemSwapChain* m_remSwapChain;
    std::vector<RemMaterial*> m_loadedShaders{};
    VkDevice m_logicalDevice;
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
