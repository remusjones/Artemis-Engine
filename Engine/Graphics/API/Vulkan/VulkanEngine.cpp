//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "VulkanEngine.h"

#include <imgui_impl_vulkan.h>

#include "GraphicsPipeline.h"
#include "VulkanGraphicsImpl.h"
#include <iostream>

#include "imgui.h"
#include "Base/Common/Material.h"
#include "Base/Common/Data/GPUCameraData.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "glog/logging.h"
#include "Scenes/Scene.h"

void VulkanEngine::Initialize(VkDevice &aLogicalDevice,
                              VulkanSwapChain *aSwapChain,
                              VkPhysicalDevice &aPhysicalDevice,
                              VkQueue &aGraphicsQueue,
                              VkQueue &aPresentQueue
) {
    // Register the shader module
    mLogicalDevice = aLogicalDevice;
    mSwapChain = aSwapChain;
    mGraphicsQueue = aGraphicsQueue;
    mPresentQueue = aPresentQueue;
    mPhysicalDevice = aPhysicalDevice;

    // Configuration info population
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProperties);
    mFrameData.resize(MAX_FRAMES_IN_FLIGHT);
    CreateDescriptorPool();
    CreateCommandPool();
}

void VulkanEngine::Cleanup() {
    CleanupOldSyncObjects();

    // Destroy Frame data
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(mLogicalDevice, mFrameData[i].mPresentSemaphore,
                           nullptr);
        vkDestroySemaphore(mLogicalDevice, mFrameData[i].mRenderSemaphore,
                           nullptr);
        vkDestroyFence(mLogicalDevice, mFrameData[i].mRenderFence, nullptr);

        vkDestroyCommandPool(mLogicalDevice, mFrameData[i].mCommandPool, nullptr);

        mFrameData[i].mCameraBuffer.Destroy();
        mFrameData[i].mLightingBuffer.Destroy();
    }
    vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool, nullptr);
    vkDestroyRenderPass(mLogicalDevice, mSwapChain->mRenderPass, nullptr);
}

void VulkanEngine::CreateCommandPool() {
    LOG(INFO) << "Creating Command Pool";
    for (int i = 0; i < mFrameData.size(); i++) {
        auto queueFamilies = gGraphics->GetQueueFamilyIndices();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilies.mGraphicsFamily.value();

        if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mFrameData[i].mCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool");
        }
    }
    CreateCommandBuffers();
}

void VulkanEngine::CreateCommandBuffers() {
    for (int i = 0; i < mFrameData.size(); i++) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mFrameData[i].mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, &mFrameData[i].mCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }
}

AllocatedBuffer VulkanEngine::CreateBuffer(size_t aAllocSize, VkBufferUsageFlags aUsage,
                                           VmaMemoryUsage vmaMemoryUsage) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;

    bufferInfo.size = aAllocSize;
    bufferInfo.usage = aUsage;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = vmaMemoryUsage;

    AllocatedBuffer newBuffer;
    vmaCreateBuffer(gGraphics->mAllocator, &bufferInfo,
                    &vmaAllocInfo, &newBuffer.mBuffer,
                    &newBuffer.mAllocation, nullptr);

    return newBuffer;
}


void VulkanEngine::CreateDescriptorPool() {
    //create a descriptor pool that will hold 10 uniform buffers
    std::vector<VkDescriptorPoolSize> sizes =
    {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}
    };

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 10;
    createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    createInfo.pPoolSizes = sizes.data();

    vkCreateDescriptorPool(mLogicalDevice, &createInfo, nullptr, &mDescriptorPool);

    for (int i = 0; i < mFrameData.size(); i++) {
        mFrameData[i].mCameraBuffer = CreateBuffer(sizeof(GPUCameraData),
                                                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                   VMA_MEMORY_USAGE_CPU_TO_GPU);

        mFrameData[i].mLightingBuffer = CreateBuffer(sizeof(GPUSceneData),
                                                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                     VMA_MEMORY_USAGE_CPU_TO_GPU);
    }
}

bool semaphoresNeedToBeRecreated = false;

void VulkanEngine::DrawFrame(Scene &aActiveScene) {
    FrameData currentFrameData = mFrameData[mCurrentFrame];
    vkWaitForFences(mLogicalDevice, 1, &currentFrameData.mRenderFence, VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkCommandBuffer currentCommandBuffer = currentFrameData.mCommandBuffer;
    VkResult result = vkAcquireNextImageKHR(mLogicalDevice,
                                            mSwapChain->mSwapChain,
                                            0,
                                            currentFrameData.mPresentSemaphore,
                                            VK_NULL_HANDLE,
                                            &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapChain->RecreateSwapChain();
        CreateCommandBuffers();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mLogicalDevice, 1, &currentFrameData.mRenderFence);
    vkResetCommandBuffer(currentCommandBuffer, 0);


    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;
    VkClearValue clearValues[] = {clearColor, depthClear};

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = gGraphics->mSwapChain->mRenderPass;
    renderPassInfo.framebuffer = gGraphics->mSwapChain->mSwapChainFrameBuffers[
        imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = gGraphics->mSwapChain->mSwapChainExtent;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = &clearValues[0];


    // ----BEGIN FRAME----
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) gGraphics->mSwapChain->mSwapChainExtent.width;
    viewport.height = (float) gGraphics->mSwapChain->mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = gGraphics->mSwapChain->mSwapChainExtent;
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);


    vkCmdBeginRenderPass(currentCommandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    aActiveScene.Render(currentCommandBuffer, imageIndex, mCurrentFrame);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), currentCommandBuffer);
    vkCmdEndRenderPass(currentCommandBuffer);

    if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    // ----END FRAME----

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(mLogicalDevice, 1, &mImagesInFlight[imageIndex],
                        VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    mImagesInFlight[imageIndex] = currentFrameData.mRenderFence;


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {currentFrameData.mPresentSemaphore};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrameData.mCommandBuffer;

    VkSemaphore signalSemaphores[] = {currentFrameData.mRenderSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo,
                      currentFrameData.mRenderFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {mSwapChain->mSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if (mRebuildFrameBuffer) {
        mSwapChain->RecreateSwapChain();

        CreateCommandBuffers();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
        mRebuildFrameBuffer = false;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapChain->RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated) {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanEngine::CleanupOldSyncObjects() {
    for (auto &i: mRenderFinishedSemaphoresToDestroy) {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mRenderFinishedSemaphoresToDestroy.clear();

    for (auto &i: mInFlightFencesToDestroy) {
        vkDestroyFence(mLogicalDevice, i, nullptr);
    }
    mInFlightFencesToDestroy.clear();

    for (auto &i: mImageAvailableSemaphoresToDestroy) {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mImageAvailableSemaphoresToDestroy.clear();
}

void VulkanEngine::CreateSyncObjects() {
    for (auto &frameData: mFrameData) {
        mInFlightFencesToDestroy.push_back(frameData.mRenderFence);
        mImageAvailableSemaphoresToDestroy.push_back(frameData.mRenderSemaphore);
        mRenderFinishedSemaphoresToDestroy.push_back(frameData.mPresentSemaphore);
    }
    LOG(INFO) << "Creating Semaphores and Fences";
    mImagesInFlight.resize(mSwapChain->mSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                              &mFrameData[i].mPresentSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                              &mFrameData[i].mRenderSemaphore) != VK_SUCCESS ||
            vkCreateFence(mLogicalDevice, &fenceInfo, nullptr,
                          &mFrameData[i].mRenderFence) != VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
        }
    }
}
