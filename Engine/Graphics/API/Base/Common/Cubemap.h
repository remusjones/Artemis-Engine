#pragma once
#include <vulkan/vulkan_core.h>

#include "Material.h"
#include "Buffers/AllocatedImage.h"

class Cubemap : public Material {
public:
    Cubemap(const char* aMaterialName) {mMaterialName = aMaterialName;}
    void Create(MaterialBase *aBaseMaterial) override;
    void Destroy() override;

    VkSampler mSampler;
    VkDescriptorImageInfo mDescriptorImageInfo;
    VkImageView mImageView;
    AllocatedImage mAllocatedImage;

    VkDescriptorSet mDescriptorSet;

};

