//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <unordered_map>
#include <glm/vec4.hpp>
#include <vulkan/vulkan_core.h>
#include "MaterialBase.h"
#include "Buffers/AllocatedBuffer.h"

class Texture;
class Buffer;

struct MaterialProperties {
    glm::vec4 mColor = glm::vec4(1, 1, 1, 1);
    // TODO: Input Images for thes properties?
    float mSpecularStrength = 1;
    float mShininess = 32;
};

class Material : public MaterialBase {
public:
    virtual void Create(MaterialBase *aBaseMaterial,
                        const char *aMaterialName = "");

    void CreateProperties(const uint32_t aBinding, const MaterialProperties &aMaterialProperties);

    void BindTexture(Texture &aTexture, uint8_t aBinding);

    void AddBinding(const uint32_t aBinding, const uint32_t aCount,
                    const VkDescriptorType aType, VkShaderStageFlagBits aShaderStage);

    void SetBuffers(const AllocatedBuffer &aBuffer, const uint8_t aBinding, const uint8_t aIndex);

    VkDescriptorSet GetDescriptorSet() const { return mDescriptorSet; }
    VkDescriptorSetLayout GetDescriptorLayout() const { return mLayout; }

    virtual void Destroy();

    MaterialProperties mMaterialProperties;
    AllocatedBuffer mPropertiesBuffer;

protected:
    const MaterialBase *mMaterialBase;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet;

    std::unordered_map<uint8_t, VkWriteDescriptorSet> mBoundTextures;
};
