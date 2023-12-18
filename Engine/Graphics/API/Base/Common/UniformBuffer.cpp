#include "UniformBuffer.h"

//
// Created by Remus on 18/12/2023.
//
UniformBuffer::UniformBuffer()
        : uniformBuffers(VulkanPipelineManager::MAX_FRAMES_IN_FLIGHT), uniformBuffersMapped(VulkanPipelineManager::MAX_FRAMES_IN_FLIGHT)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for (size_t i = 0; i < VulkanPipelineManager::MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto* uniformBuffer = new Buffer();
        uniformBuffer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer->mBuffer, uniformBuffer->mMemory);

        vkMapMemory(gGraphics->mLogicalDevice, uniformBuffer->mMemory, 0, bufferSize, 0, &uniformBuffersMapped[i]);

        uniformBuffers[i] = uniformBuffer;
    }
}

void UniformBuffer::Destroy() {
    for (auto & uniformBuffer : uniformBuffers)
    {
        uniformBuffer->Destroy();
    }
    uniformBuffers.resize(0, nullptr);
    uniformBuffers.resize(0, nullptr);
}
