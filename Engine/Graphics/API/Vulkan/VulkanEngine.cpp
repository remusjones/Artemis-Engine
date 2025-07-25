//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "VulkanEngine.h"

#include <imgui_impl_vulkan.h>

#include "VulkanGraphicsImpl.h"
#include <iostream>

#include "imgui.h"
#include <Logger.h>
#include "Base/Common/Material.h"
#include "Base/Common/Data/GPUCameraData.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Helpers/VulkanInitialization.h"
#include "Scenes/Scene.h"

void VulkanEngine::Initialize(VkDevice& aLogicalDevice,
                              VulkanSwapChain* aSwapChain,
                              VkPhysicalDevice& aPhysicalDevice,
                              VkQueue& aGraphicsQueue,
                              VkQueue& aPresentQueue
)
{
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

void VulkanEngine::Cleanup()
{
    CleanupOldSyncObjects();

    vkDestroyCommandPool(mLogicalDevice, mUploadContext.mCommandPool, nullptr);

    // Destroy Frame data
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(mLogicalDevice, mFrameData[i].mPresentSemaphore,
                           nullptr);
        vkDestroySemaphore(mLogicalDevice, mFrameData[i].mRenderSemaphore,
                           nullptr);
        vkDestroyFence(mLogicalDevice, mFrameData[i].mRenderFence, nullptr);

        vkDestroyCommandPool(mLogicalDevice, mFrameData[i].mCommandPool, nullptr);

        //mFrameData[i].mCameraBuffer.Destroy();
        mFrameData[i].mSceneBuffer.Destroy();
    }
    vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool, nullptr);
    vkDestroyRenderPass(mLogicalDevice, mSwapChain->mRenderPass, nullptr);
}

void VulkanEngine::SubmitBufferCommand(std::function<void(VkCommandBuffer cmd)>&& function) const
{
    VkCommandBuffer cmd = mUploadContext.mCommandBuffer;
    const VkCommandBufferBeginInfo cmdBeginInfo = VulkanInitialization::CommandBufferBeginInfo(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    vkBeginCommandBuffer(cmd, &cmdBeginInfo);
    function(cmd);
    vkResetFences(mLogicalDevice, 1, &mUploadContext.mUploadFence);
    vkEndCommandBuffer(cmd);

    const VkSubmitInfo submit = VulkanInitialization::SubmitInfo(&cmd);


    vkQueueSubmit(mGraphicsQueue, 1, &submit, mUploadContext.mUploadFence);
    vkWaitForFences(mLogicalDevice, 1, &mUploadContext.mUploadFence, true, 9999999999);
    vkResetFences(mLogicalDevice, 1, &mUploadContext.mUploadFence);
    vkResetCommandPool(mLogicalDevice, mUploadContext.mCommandPool, 0);
}
void VulkanEngine::SubmitEndOfFrameTask(std::function<void()> && aTask){
    mEndOfFrameTasks.emplace(std::move(aTask));
}

void VulkanEngine::CreateUploadContext()
{
    auto queueFamilies = gGraphics->GetQueueFamilyIndices();
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilies.mGraphicsFamily.value();

    if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mUploadContext.mCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool");
    }

    const VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitialization::CommandBufferAllocateInfo(mUploadContext
                                                                                                     .mCommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    vkAllocateCommandBuffers(mLogicalDevice, &cmdAllocInfo, &mUploadContext.mCommandBuffer);


    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkCreateFence(mLogicalDevice, &info, nullptr, &mUploadContext.mUploadFence);
    mInFlightFencesToDestroy.push_back(mUploadContext.mUploadFence);
}

void VulkanEngine::CreateCommandPool()
{
    Logger::Log(spdlog::level::debug, "Creating Command Pool");


    for (int i = 0; i < mFrameData.size(); i++)
    {
        auto queueFamilies = gGraphics->GetQueueFamilyIndices();
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilies.mGraphicsFamily.value();
        if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mFrameData[i].mCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool");
        }
    }


    CreateUploadContext();
    CreateCommandBuffers();
}

void VulkanEngine::CreateCommandBuffers()
{
    for (int i = 0; i < mFrameData.size(); i++)
    {
        VkCommandBufferAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mFrameData[i].mCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, &mFrameData[i].mCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }
}

void VulkanEngine::DestroyCommandPool() const {
    vkDestroyCommandPool(mLogicalDevice, mUploadContext.mCommandPool, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyCommandPool(mLogicalDevice, mFrameData[i].mCommandPool, nullptr);
    }
}

void VulkanEngine::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> sizes =
    {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
    };

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 10;
    createInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    createInfo.pPoolSizes = sizes.data();

    vkCreateDescriptorPool(mLogicalDevice, &createInfo, nullptr, &mDescriptorPool);

    for (int i = 0; i < mFrameData.size(); i++)
    {
        //mFrameData[i].mCameraBuffer = CreateBuffer(sizeof(GPUCameraData),
        //                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        //                                           VMA_MEMORY_USAGE_CPU_TO_GPU);

        mFrameData[i].mSceneBuffer = AllocatedBuffer();
        mFrameData[i].mSceneBuffer.Create(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
}

bool semaphoresNeedToBeRecreated = false;

void VulkanEngine::DrawFrame(Scene& aActiveScene)
{
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


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        mSwapChain->RecreateSwapChain();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
    }
    else if (result != VK_SUCCESS)
    {
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

    if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = gGraphics->mSwapChain->mSwapChainExtent.height;;
    viewport.width = static_cast<float>(gGraphics->mSwapChain->mSwapChainExtent.width);
    viewport.height = -static_cast<float>(gGraphics->mSwapChain->mSwapChainExtent.height);
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

    if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
    // ----END FRAME----

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE)
    {
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
                      currentFrameData.mRenderFence) != VK_SUCCESS)
    {
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

    if (mRebuildFrameBuffer)
    {
        mSwapChain->RecreateSwapChain();

        DestroyCommandPool();
        CreateCommandPool();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
        mRebuildFrameBuffer = false;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        mSwapChain->RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated)
    {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }
    while(!mEndOfFrameTasks.empty()) {
        auto task = std::move(mEndOfFrameTasks.front());
        task();
        mEndOfFrameTasks.pop();
    }
    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanEngine::CleanupOldSyncObjects()
{
    for (const auto& i : mRenderFinishedSemaphoresToDestroy)
    {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mRenderFinishedSemaphoresToDestroy.clear();

    for (const auto& i : mInFlightFencesToDestroy)
    {
        vkDestroyFence(mLogicalDevice, i, nullptr);
    }
    mInFlightFencesToDestroy.clear();

    for (const auto& i : mImageAvailableSemaphoresToDestroy)
    {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mImageAvailableSemaphoresToDestroy.clear();
}

void VulkanEngine::CreateSyncObjects()
{
    for (auto& frameData : mFrameData)
    {
        mInFlightFencesToDestroy.push_back(frameData.mRenderFence);
        mImageAvailableSemaphoresToDestroy.push_back(frameData.mRenderSemaphore);
        mRenderFinishedSemaphoresToDestroy.push_back(frameData.mPresentSemaphore);
    }
    Logger::Log(spdlog::level::debug, "Creating Semaphores and Fences");
    mImagesInFlight.resize(mSwapChain->mSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                              &mFrameData[i].mPresentSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr,
                              &mFrameData[i].mRenderSemaphore) != VK_SUCCESS ||
            vkCreateFence(mLogicalDevice, &fenceInfo, nullptr,
                          &mFrameData[i].mRenderFence) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "failed to create synchronization objects for a frame!");
        }
    }
}
