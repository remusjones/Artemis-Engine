//
// Created by Remus on 27/12/2023.
//

#pragma once
#include <string>
#include <vector>
#include "AllocatedImage.h"

struct Color_RGBA;

class Texture {
public:
    Texture() = default;
    void LoadImagesFromDisk(const std::vector<std::string>& aPaths);

    void Create(VkFilter aSamplerFilter = VK_FILTER_LINEAR,
                VkSamplerAddressMode aSamplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);


    void Destroy() const;

    AllocatedImage mAllocatedImage;
    uint32_t mImageCount = 1;
    std::vector<VkImageView> mImageViews;
    VkSampler mSampler;
    std::vector<VkDescriptorImageInfo> mImageBufferInfo;
};
