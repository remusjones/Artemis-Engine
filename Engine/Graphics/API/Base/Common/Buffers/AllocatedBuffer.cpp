//
// Created by Remus on 20/12/2023.
//

#include "AllocatedBuffer.h"

#include "Logger.h"
#include "VulkanGraphicsImpl.h"

AllocatedBuffer::AllocatedBuffer(const void *aData, VkDeviceSize aBufferSize,
                                 VkBufferUsageFlags aUsageFlags) {
    void *data;
    Create(aBufferSize, aUsageFlags, mBuffer, mAllocation);
    //copy vertex data
    vmaMapMemory(gGraphics->mAllocator, mAllocation, &data);
    memcpy(data, aData, aBufferSize);
    vmaUnmapMemory(gGraphics->mAllocator, mAllocation);

}

AllocatedBuffer::~AllocatedBuffer() {
}

void AllocatedBuffer::Create(VkDeviceSize aSize,
                             VkBufferUsageFlags aUsage,
                             VkBuffer &aBuffer, VmaAllocation &aAllocation) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = aSize;
    bufferInfo.usage = aUsage;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    vmaallocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                         VMA_ALLOCATION_CREATE_MAPPED_BIT;

    vmaCreateBuffer(gGraphics->mAllocator, &bufferInfo, &vmaallocInfo,
                    &aBuffer,
                    &aAllocation,
                    nullptr);
}

void AllocatedBuffer::Destroy() {
    vmaDestroyBuffer(gGraphics->mAllocator, mBuffer, mAllocation);
}
