//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vulkan/vulkan_core.h>
#include "MaterialBase.h"

class Buffer;

class Material : public MaterialBase {
public:
    void Create(const MaterialBase *aBaseMaterial, const char *aMaterialName);

    void AddBinding(const uint32_t aBinding, const uint32_t aCount,
                    const VkDescriptorType aType, VkShaderStageFlagBits aShaderStage);

    void Destroy();

private:
    const MaterialBase *mMaterialBase;
    VkDescriptorSet nDescriptorSet = VK_NULL_HANDLE;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
};
