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
    glfwGetFramebufferSize(m_remApplicationInstance->mWindow, &width, &height);
    while (width == 0 || height == 0) {

        glfwGetFramebufferSize(m_remApplicationInstance->mWindow, &width, &height);
        glfwWaitEvents();
        std::cout << "VulkanApplicationImpl Minimized" << std::endl;
    }

    vkDeviceWaitIdle(m_logicalDevice);
    Cleanup();
    CreateSwapChain();
    CreateImageViews();
    CreateFrameBuffers();
}

void VulkanSwapChain::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = m_remApplicationInstance->QuerySwapChainSupport(m_physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = m_remApplicationInstance->ChooseSwapSurfaceFormat(swapChainSupport.mFormats);
    VkPresentModeKHR presentMode = m_remApplicationInstance->ChooseSwapPresentMode(swapChainSupport.mPresentModes);
    VkExtent2D extent = m_remApplicationInstance->ChooseSwapExtent(swapChainSupport.mCapabilities);

    uint32_t imageCount = swapChainSupport.mCapabilities.minImageCount + 1;

    if (swapChainSupport.mCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.mCapabilities.maxImageCount) {
        imageCount = swapChainSupport.mCapabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = m_remApplicationInstance->FindQueueFamilies(m_physicalDevice);
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



    if (vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, m_swapChainImages.data());
}


void VulkanSwapChain::CreateFrameBuffers()
{
    std::cout << "\tCreating Frame Buffers" << std::endl;
    m_swapChainFrameBuffers.resize(m_swapChainImageViews.size());
    for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
                m_swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_logicalDevice, &framebufferInfo, nullptr, &m_swapChainFrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

}

void VulkanSwapChain::CreateImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());
    for (size_t i = 0; i < m_swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VulkanSwapChain::Cleanup()
{
    std::cout << "Destroying Frame Buffer" << std::endl;
    for (auto & m_swapChainFrameBuffer : m_swapChainFrameBuffers) {
        vkDestroyFramebuffer(m_logicalDevice, m_swapChainFrameBuffer, nullptr);
    }

    for (auto & m_swapChainImageView : m_swapChainImageViews) {
        vkDestroyImageView(m_logicalDevice, m_swapChainImageView, nullptr);
    }
    vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
}

void VulkanSwapChain::CreateRenderPass()
{
    std::cout << "Creating Render Pass" << std::endl;
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainImageFormat;
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

    if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

}

void VulkanSwapChain::Initialize(VkDevice &mLogicalDevice,
                                 VkPhysicalDevice &mPhysicalDevice,
                                 VkSurfaceKHR &mSurface,
                                 VkRenderPass& renderPass,
                                 VulkanApplicationImpl *remWindow)
{
    m_logicalDevice = mLogicalDevice;
    m_physicalDevice = mPhysicalDevice;
    m_surface = mSurface;
    m_renderPass = renderPass;
    m_remApplicationInstance = remWindow;

    std::cout << "Constructing Swap Chain" << std::endl;
    CreateSwapChain();
    std::cout << "Constructing Image Views" << std::endl;
    CreateImageViews();

    CreateRenderPass();
}
