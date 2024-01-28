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

    void Create(VkDeviceSize aSize, VkBufferUsageFlags aUsage, VkBuffer &aBuffer,
                      VmaAllocation &aAllocation);
    void AllocateBuffer(const void* aData, VkDeviceSize aBufferSize, VkBufferUsageFlags aUsageFlags);
    [[nodiscard]] bool IsAllocted() const;

    void Destroy();

    VkBuffer mBuffer = nullptr;
    VmaAllocation mAllocation = nullptr;
};
