#pragma once
#include <vulkan/vulkan_core.h>

#include "Material.h"
#include "Buffers/AllocatedImage.h"

class Cubemap : public Material {
public:
    void Create(MaterialBase *aBaseMaterial, const char *aMaterialName) override;
    void Destroy() override;

    VkSampler mSampler;
    VkDescriptorImageInfo mDescriptorImageInfo;
    VkImageView mImageView;
    AllocatedImage mAllocatedImage;

    VkDescriptorSet mDescriptorSet;

};

