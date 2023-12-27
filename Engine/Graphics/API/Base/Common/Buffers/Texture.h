//
// Created by Remus on 27/12/2023.
//

#pragma once
#include "AllocatedImage.h"
#include "LoadUtilities.h"
#include "VulkanGraphicsImpl.h"

class Texture {
public:
    Texture() = default;
    void LoadImageFromDisk(const char* aFilePath) {  LoadUtilities::LoadImageFromDisk(gGraphics, aFilePath,
    mAllocatedImage);}
    void Create(VkFilter aSamplerFilter = VK_FILTER_LINEAR,
                VkSamplerAddressMode aSamplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

    void Destroy() const;

    AllocatedImage mAllocatedImage;
    VkImageView mImageView;
    VkSampler mSampler;
    VkDescriptorImageInfo mImageBufferInfo;
};
