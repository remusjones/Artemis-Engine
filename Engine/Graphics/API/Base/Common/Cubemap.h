#pragma once
#include <vulkan/vulkan_core.h>

#include "Buffers/AllocatedImage.h"

class Cubemap {
    void Create();
    void Destroy();

    VkSampler mSampler;
    VkDescriptorImageInfo mDescriptorImageInfo;
    VkImageView mImageView;
    VkImageLayout mImageLayout;
    AllocatedImage mAllocatedImage;
};

