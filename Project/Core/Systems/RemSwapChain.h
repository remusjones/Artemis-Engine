//
// Created by Remus on 11/12/2022.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifndef INC_3DENGINE_REMSWAPCHAIN_H
#define INC_3DENGINE_REMSWAPCHAIN_H

#include <vector>
#include <optional>
class RemWindow;

class RemSwapChain {
public:
    RemSwapChain(VkDevice &mLogicalDevice,
                 VkPhysicalDevice &mPhysicalDevice,
                 VkSurfaceKHR &mSurface,
                 VkRenderPass& renderPass,
                 RemWindow *remWindow);

    void Initialize();

    void RecreateSwapChain();

    void CreateSwapChain();

    void CreateFrameBuffers();

    void CreateImageViews();

    void Cleanup();

    VkPhysicalDevice &m_physicalDevice;
    RemWindow* m_remWindowInstance{};
    // swapchain
    VkSwapchainKHR m_swapChain{};
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent{};
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    VkDevice m_logicalDevice;
    VkSurfaceKHR m_surface;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkRenderPass& m_renderPass;
};

#endif //INC_3DENGINE_REMSWAPCHAIN_H
