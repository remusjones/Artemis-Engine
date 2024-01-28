//
// Created by Remus on 20/12/2023.
//

#include "AllocatedBuffer.h"

#include "Logger.h"
#include "VulkanGraphicsImpl.h"

AllocatedBuffer::AllocatedBuffer(const void *aData, const VkDeviceSize aBufferSize,
                                 const VkBufferUsageFlags aUsageFlags) {
    void *data;
    Create(aBufferSize, aUsageFlags);
    //copy vertex data
    vmaMapMemory(gGraphics->mAllocator, mAllocation, &data);
    memcpy(data, aData, aBufferSize);
    vmaUnmapMemory(gGraphics->mAllocator, mAllocation);
}

AllocatedBuffer::~AllocatedBuffer() {
}

void AllocatedBuffer::AllocateBuffer(const void *aData, const VkDeviceSize aBufferSize,
                                     const VkBufferUsageFlags aUsageFlags) {
    void *data;
    Create(aBufferSize, aUsageFlags);
    vmaMapMemory(gGraphics->mAllocator, mAllocation, &data);
    memcpy(data, aData, aBufferSize);
    vmaUnmapMemory(gGraphics->mAllocator, mAllocation);
}

void AllocatedBuffer::MapMemory(const VmaAllocator aVmaAllocator, const void *aData, VmaAllocation aAllocation, VkDeviceSize aSize) {
    void *data;
    vmaMapMemory(aVmaAllocator, aAllocation, &data);
    memcpy(data, aData, aSize);
    vmaUnmapMemory(aVmaAllocator, aAllocation);
}

VkBuffer AllocatedBuffer::GetBuffer() const { return mBuffer; }

VmaAllocation AllocatedBuffer::GetAllocation() const { return mAllocation; }

bool AllocatedBuffer::IsAllocted() const {
    return mBuffer != nullptr || mAllocation != nullptr;
}

void AllocatedBuffer::Create(const VkDeviceSize aSize, const VkBufferUsageFlags aUsage) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = aSize;
    bufferInfo.usage = aUsage;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    vmaallocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                         VMA_ALLOCATION_CREATE_MAPPED_BIT;
    if (const VkResult result = vmaCreateBuffer(gGraphics->mAllocator, &bufferInfo, &vmaallocInfo,
                                                &mBuffer,
                                                &mAllocation,nullptr); result != VK_SUCCESS) {
        Logger::Log(spdlog::level::critical, "Failed to create AllocatedBuffer");
                                                }
}


void AllocatedBuffer::Destroy() {
    vmaDestroyBuffer(gGraphics->mAllocator, mBuffer, mAllocation);
    mBuffer = nullptr;
    mAllocation = nullptr;
}
