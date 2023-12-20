//
// Created by Remus on 20/12/2023.
//

#include "AllocatedBuffer.h"

#include "VulkanGraphicsImpl.h"

AllocatedBuffer::AllocatedBuffer(const void *aData, VkDeviceSize aBufferSize,
    VkBufferUsageFlags aUsageFlags) {

    void* data;
    CreateBuffer(aBufferSize, aUsageFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,mBuffer, mAllocation);
    //copy vertex data
    vmaMapMemory(gGraphics->mAllocator, mAllocation, &data);
    memcpy(data, aData, aBufferSize);
    vmaUnmapMemory(gGraphics->mAllocator, mAllocation);


}

AllocatedBuffer::~AllocatedBuffer() {
    vmaDestroyBuffer(gGraphics->mAllocator, mBuffer, mAllocation);
}

void AllocatedBuffer::CreateBuffer(VkDeviceSize aSize,
                                   VkBufferUsageFlags aUsage, VkMemoryPropertyFlags aProperties,
                                   VkBuffer&aBuffer, VmaAllocation &aAllocation) {


    //allocate vertex buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //this is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = aSize;
    //this buffer is going to be used as a Vertex Buffer
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
