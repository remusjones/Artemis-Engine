//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vulkan/vulkan_core.h>
#include "MaterialBase.h"

class AllocatedBuffer;
class Buffer;

class Material : public MaterialBase {
public:
    void Create(const MaterialBase *aBaseMaterial, const char *aMaterialName);

    void AddBinding(const uint32_t aBinding, const uint32_t aCount,
                    const VkDescriptorType aType, VkShaderStageFlagBits aShaderStage);

    void SetBuffers(const AllocatedBuffer& aBuffer, const uint8_t aBinding, const uint8_t aIndex);

    VkDescriptorSet GetDescriptorSet() const {return mDescriptorSet;}
    VkDescriptorSetLayout GetDescriptorLayout() const {return mLayout;}
    void Destroy();

private:
    const MaterialBase *mMaterialBase;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet;
};
