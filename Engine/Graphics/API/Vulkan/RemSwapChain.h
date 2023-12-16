//
// Created by Remus on 11/12/2022.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifndef INC_3DENGINE_REMSWAPCHAIN_H
#define INC_3DENGINE_REMSWAPCHAIN_H

#include <vector>
#include <optional>
class RemApplication;

class RemSwapChain {
public:



    void Initialize(VkDevice &mLogicalDevice,
                    VkPhysicalDevice &mPhysicalDevice,
                    VkSurfaceKHR &mSurface,
                    VkRenderPass& renderPass,
                    RemApplication *remWindow);

    void RecreateSwapChain();
    void CreateSwapChain();
    void CreateFrameBuffers();
    void CreateImageViews();
    void Cleanup();

    VkPhysicalDevice m_physicalDevice;
    RemApplication* m_remApplicationInstance{};
    // swapchain
    VkSwapchainKHR m_swapChain{};
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent{};
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    VkDevice m_logicalDevice;
    VkSurfaceKHR m_surface;
    std::vector<VkFramebuffer> m_swapChainFrameBuffers;
    VkRenderPass m_renderPass;

    void CreateRenderPass();
};

#endif //INC_3DENGINE_REMSWAPCHAIN_H
