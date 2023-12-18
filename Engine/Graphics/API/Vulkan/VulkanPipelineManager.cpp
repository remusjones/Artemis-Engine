//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "VulkanPipelineManager.h"
#include "GraphicsPipeline.h"
#include <iostream>
void VulkanPipelineManager::Initialize(VkDevice& aLogicalDevice,
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
}

void VulkanPipelineManager::Cleanup()
{

    CleanupOldSyncObjects();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(mLogicalDevice, mInFlightFences[i], nullptr);
    }
    if (mCommandPool != nullptr)
    {
        vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);
    }else
    {
        throw std::runtime_error("Attempted to Deconstruct Command Pool, but there was none to deconstruct");
    }

    vkDestroyRenderPass(mLogicalDevice, mSwapChain->mRenderPass, nullptr);
    // release all loaded shaders

    for(auto & loadedMaterial : mLoadedMaterials)
    {
        VulkanMaterial* targetMaterial = loadedMaterial;

        targetMaterial->CleanupShaderModules(mLogicalDevice);
        delete loadedMaterial;
    }

    mLoadedMaterials.resize(0, nullptr);

    for(auto& graphicsPipeline : mGraphicsPipelines)
    {
        graphicsPipeline->Destroy();
        delete graphicsPipeline;
    }
    mGraphicsPipelines.resize(0, nullptr);
}

void VulkanPipelineManager::CreateCommandPool(const QueueFamilyIndices& aQueueFamilyIndices)
{
    std::cout << "Creating Command Pool" << std::endl;
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = aQueueFamilyIndices.mGraphicsFamily.value();

    if (vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
    CreateCommandBuffers();
}

void VulkanPipelineManager::CreateCommandBuffers()
{
    mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();

    if (vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers");
    }
}

bool semaphoresNeedToBeRecreated = false;

void VulkanPipelineManager::DrawFrame()
{
    vkWaitForFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;

    //
    // Avoid max timout due to weird issue (after swap chain recreation) - https://www.reddit.com/r/vulkan/comments/wfugbz/weird_behaviour_of_vkacquireimagekhr/
    // 14/12/2022
    //



    VkResult result = vkAcquireNextImageKHR(mLogicalDevice,
                                            mSwapChain->mSwapChain,
                                            0,
                                            mImageAvailableSemaphores[mCurrentFrame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapChain->RecreateSwapChain();

        CreateCommandBuffers();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;

    }else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Only reset the fence if we are submitting work
    vkResetFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame]);

    vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    // VUID-vkCmdDrawIndexed-None-08608
    //RecordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

    for(auto & mGraphicsPipeline : mGraphicsPipelines)
    {
        mGraphicsPipeline->RenderPipeline(mCommandBuffers[mCurrentFrame], imageIndex);
    }

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (mImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(mLogicalDevice, 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];


    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

    VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) {
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

    if (mFramebufferResized)
    {
        mSwapChain->RecreateSwapChain();

        CreateCommandBuffers();
        // Sync objects aren't atomic, so we have to regenerate them at the end of the current frame
        semaphoresNeedToBeRecreated = true;
        mFramebufferResized = false;
    }
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        mSwapChain->RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    if (semaphoresNeedToBeRecreated)
    {
        CreateSyncObjects();
        semaphoresNeedToBeRecreated = false;
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}

void VulkanPipelineManager::CleanupOldSyncObjects()
{
    for (auto & i : mRenderFinishedSemaphoresToDestroy) {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mRenderFinishedSemaphoresToDestroy.clear();

    for (auto & i : mInFlightFencesToDestroy) {
        vkDestroyFence(mLogicalDevice, i, nullptr);
    }
    mInFlightFencesToDestroy.clear();

    for (auto & i : mImageAvailableSemaphoresToDestroy) {
        vkDestroySemaphore(mLogicalDevice, i, nullptr);
    }
    mImageAvailableSemaphoresToDestroy.clear();
}

void VulkanPipelineManager::CreateSyncObjects()
{
    mInFlightFencesToDestroy.insert(mInFlightFencesToDestroy.end(), std::begin(mInFlightFences), std::end(mInFlightFences));     // C++11
    mImageAvailableSemaphoresToDestroy.insert(mImageAvailableSemaphoresToDestroy.end(), std::begin(mImageAvailableSemaphores), std::end(mImageAvailableSemaphores));     // C++11
    mRenderFinishedSemaphoresToDestroy.insert(mRenderFinishedSemaphoresToDestroy.end(), std::begin(mRenderFinishedSemaphores), std::end(mRenderFinishedSemaphores));     // C++11


    std::cout << "Creating Semaphores and Fences" << std::endl;
    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    mImagesInFlight.resize(mSwapChain->mSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(mLogicalDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanPipelineManager::AddGraphicsPipeline(GraphicsPipeline* aGraphicsPipeline) {
    mGraphicsPipelines.push_back(aGraphicsPipeline);
}




