//
// Created by Remus on 11/12/2022.
//
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>

class VulkanApplicationImpl;

class VulkanSwapChain {
public:

    void Initialize(VkDevice& aLogicalDevice,
                    VkPhysicalDevice& aPhysicalDevice,
                    VkSurfaceKHR& aSurface,
                    VkRenderPass& aRenderPass,
                    VulkanApplicationImpl *aWindow);

    void RecreateSwapChain();
    void CreateSwapChain();
    void CreateFrameBuffers();
    void CreateImageViews();
    void CreateRenderPass();
    void Cleanup();

    VkPhysicalDevice mPhysicalDevice;
    VulkanApplicationImpl* mApplication{};
    VkSwapchainKHR mSwapChain{};
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent{};
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;
    VkDevice mLogicalDevice;
    VkSurfaceKHR mSurface;
    std::vector<VkFramebuffer> mSwapChainFrameBuffers;
    VkRenderPass mRenderPass;

};

