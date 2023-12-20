//
// Created by Remus on 20/12/2023.
//

#pragma once
#include "AllocatedVertexBuffer.h"

class AllocatedBuffer {
public:
    AllocatedBuffer() = default;
    AllocatedBuffer(const void *aData, VkDeviceSize aBufferSize,
    VkBufferUsageFlags aUsageFlags);
    ~AllocatedBuffer();

    void CreateBuffer(VkDeviceSize aSize, VkBufferUsageFlags aUsage, VkMemoryPropertyFlags aProperties, VkBuffer &aBuffer,
                      VmaAllocation &aAllocation);

    VkBuffer mBuffer;
    VmaAllocation mAllocation;
};
