//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>


class VulkanPipelineManager;

class Buffer {
public:
    Buffer(const void *aData, VkDeviceSize bufferSize, VkBufferUsageFlags aUsage);
    Buffer() = default;
    ~Buffer();

    void CreateBuffer(VkDeviceSize aSize, VkBufferUsageFlags aUsage,
                                              VkMemoryPropertyFlags aProperties, VkBuffer& aBuffer, VkDeviceMemory&
                                              aBufferMemory);

    void CopyBuffer(VkBuffer aSrcBuffer, VkBuffer aDstBuffer, VkDeviceSize aSize);
    uint32_t FindMemoryType(uint32_t aTypeFilter, VkMemoryPropertyFlags aProperties);

    VkBuffer mBuffer;
    VkDeviceMemory mMemory;
};
