//
// Created by Remus on 11/12/2022.
//

#include <stdexcept>
#include <iostream>
#include "VulkanSwapChain.h"
#include "VulkanSystemStructs.h"
#include "VulkanApplicationImpl.h"

void VulkanSwapChain::RecreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(mApplication->mWindow, &width, &height);
    while (width == 0 || height == 0) {

        glfwGetFramebufferSize(mApplication->mWindow, &width, &height);
        glfwWaitEvents();
        std::cout << "VulkanApplicationImpl Minimized" << std::endl;
    }

    vkDeviceWaitIdle(mLogicalDevice);
    Cleanup();
    CreateSwapChain();
    CreateImageViews();
    CreateFrameBuffers();
}

void VulkanSwapChain::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = mApplication->QuerySwapChainSupport(mPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = mApplication->ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
    VkPresentModeKHR presentMode = mApplication->ChooseSwapPresentMode(swapChainSupport.mPresentModes);
    VkExtent2D extent = mApplication->ChooseSwapExtent(swapChainSupport.mCapabilities);

    uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;

    if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.mCapabilities.maxImageCount) {
        imageCount = swapChainSupport.mCapabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = mApplication->FindQueueFamilies(mPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.mGraphicsFamily.value(), indices.mPresentFamily.value()};

    if (indices.mGraphicsFamily != indices.mPresentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.mCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;



    if (vkCreateSwapchainKHR(mLogicalDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    mSwapChainImageFormat = surfaceFormat.format;
    mSwapChainExtent = extent;
    mSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, mSwapChainImages.data());
}


void VulkanSwapChain::CreateFrameBuffers()
{
    std::cout << "\tCreating Frame Buffers" << std::endl;
    mSwapChainFrameBuffers.resize(mSwapChainImageViews.size());
    for (size_t i = 0; i < mSwapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
                mSwapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = mSwapChainExtent.width;
        framebufferInfo.height = mSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(mLogicalDevice, &framebufferInfo, nullptr, &mSwapChainFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

}

void VulkanSwapChain::CreateImageViews()
{
    mSwapChainImageViews.resize(mSwapChainImages.size());
    for (size_t i = 0; i < mSwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = mSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(mLogicalDevice, &createInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VulkanSwapChain::Cleanup()
{
    std::cout << "Destroying Frame Buffer" << std::endl;
    for (auto & m_swapChainFrameBuffer : mSwapChainFrameBuffers) {
        vkDestroyFramebuffer(mLogicalDevice, m_swapChainFrameBuffer, nullptr);
    }

    for (auto & m_swapChainImageView : mSwapChainImageViews) {
        vkDestroyImageView(mLogicalDevice, m_swapChainImageView, nullptr);
    }
    vkDestroySwapchainKHR(mLogicalDevice, mSwapChain, nullptr);
}

void VulkanSwapChain::CreateRenderPass()
{
    std::cout << "Creating Render Pass" << std::endl;
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(mLogicalDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

}

void VulkanSwapChain::Initialize(VkDevice &aLogicalDevice,
                                 VkPhysicalDevice &aPhysicalDevice,
                                 VkSurfaceKHR &aSurface,
                                 VkRenderPass& aRenderPass,
                                 VulkanApplicationImpl *aWindow)
{
    mLogicalDevice = aLogicalDevice;
    mPhysicalDevice = aPhysicalDevice;
    mSurface = aSurface;
    mRenderPass = aRenderPass;
    mApplication = aWindow;

    std::cout << "Constructing Swap Chain" << std::endl;
    CreateSwapChain();
    std::cout << "Constructing Image Views" << std::endl;
    CreateImageViews();

    CreateRenderPass();
}
