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

    void Create(VkDeviceSize aSize, VkBufferUsageFlags aUsage);

    void AllocateBuffer(const void* aData, VkDeviceSize aBufferSize, VkBufferUsageFlags aUsageFlags);

    // Copy void* data to buffer
    static void MapMemory(VmaAllocator aVmaAllocator, const void *aData, VmaAllocation aAllocation, VkDeviceSize aSize);

    [[nodiscard]] VkBuffer GetBuffer() const;
    [[nodiscard]] VmaAllocation GetAllocation() const;
    [[nodiscard]] bool IsAllocated() const;

    void Destroy();

private:
    VkBuffer mBuffer = nullptr;
    VmaAllocation mAllocation = nullptr;
};
