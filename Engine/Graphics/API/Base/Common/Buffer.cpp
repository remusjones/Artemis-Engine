//
// Created by Remus on 17/12/2023.
//

#include "Buffer.h"
#include "VulkanGraphicsImpl.h"

Buffer::Buffer(const void *aData, VkDeviceSize bufferSize, VkBufferUsageFlags aUsage) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(gGraphics->mLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, aData, (size_t) bufferSize);
    vkUnmapMemory(gGraphics->mLogicalDevice, stagingBufferMemory);

    CreateBuffer(bufferSize, aUsage,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mBuffer, mMemory);

    CopyBuffer(stagingBuffer, mBuffer, bufferSize);

    vkDestroyBuffer(gGraphics->mLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(gGraphics->mLogicalDevice, stagingBufferMemory, nullptr);
}
void Buffer::CreateBuffer(VkDeviceSize aSize, VkBufferUsageFlags aUsage, VkMemoryPropertyFlags aProperties, VkBuffer &aBuffer,
                          VkDeviceMemory &aBufferMemory) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = aSize;
    bufferInfo.usage = aUsage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gGraphics->mLogicalDevice, &bufferInfo, nullptr, &aBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create aBuffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(gGraphics->mLogicalDevice, aBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, aProperties);

    if (vkAllocateMemory(gGraphics->mLogicalDevice, &allocInfo, nullptr, &aBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate aBuffer memory!");
    }

    vkBindBufferMemory(gGraphics->mLogicalDevice, aBuffer, aBufferMemory, 0);
}

uint32_t Buffer::FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gGraphics->mPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((aTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & aProperties) == aProperties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");

}

void Buffer::CopyBuffer(VkBuffer aSrcBuffer, VkBuffer aDstBuffer, VkDeviceSize aSize)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = gGraphics->mRenderPipeline.mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(gGraphics->mLogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = aSize;
    vkCmdCopyBuffer(commandBuffer, aSrcBuffer, aDstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(gGraphics->mRenderPipeline.mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gGraphics->mRenderPipeline.mGraphicsQueue);

    vkFreeCommandBuffers(gGraphics->mLogicalDevice, gGraphics->mRenderPipeline.mCommandPool, 1, &commandBuffer);

}

void Buffer::Cleanup() {
    if (mBuffer)
        vkDestroyBuffer(gGraphics->mLogicalDevice, mBuffer, nullptr);
    if (mMemory)
        vkFreeMemory(gGraphics->mLogicalDevice, mMemory, nullptr);
}
