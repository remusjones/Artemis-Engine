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
#include "RemShaderComponent.h"

class RemPipeline
{
public:

    void Initialize(VkDevice& logicalDevice,
                    VkSwapchainKHR& swapChainKhr,
                    VkExtent2D& swapChainExtent,
                    VkFormat& swapChainImageFormat,
                    std::vector<VkImageView>& swapChainImageViews,
                    std::vector<VkImage>& swapChainImages,
                    VkPhysicalDevice& physicalDevice,
                    VkQueue& graphicsQueue,
                    VkQueue& presentQueue
                    );

    VkShaderModule CreateShaderModule(const std::vector<char>& code);

    VkResult LoadShader(const std::string& shaderName);
    void CreatePipelineLayout();
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateCommandPool(QueueFamilyIndices& queueFamilyIndices);
    void CreateCommandBuffer();
    void DrawFrame();
    void CreateSyncObjects();

    void Cleanup();
    void DestroyShader(RemShaderComponent* shaderComponent);


    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;
private:
    std::vector<RemShaderComponent*> m_loadedShaders{};
    VkDevice m_logicalDevice;
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkSwapchainKHR m_swapChain;
    VkExtent2D m_swapChainExtent;
    VkFormat m_swapChainImageFormat;
    VkPipeline m_graphicsPipeline;
    std::vector<VkImageView> m_swapChainImageViews{};
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkFramebuffer> m_swapChainFrameBuffers{};
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers{};
    bool m_hasCreatedPipeline = false;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    std::vector<VkSemaphore>  m_imageAvailableSemaphores;
    std::vector<VkSemaphore>  m_renderFinishedSemaphores;

    size_t m_currentFrame = 0;


    const int MAX_FRAMES_IN_FLIGHT = 2;

};


#endif //INC_3DENGINE_SHADERMODULE_H
