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
    float mSpecularStrength = 0.4;
    float mShininess = 1;
    float mDebugRenderState;
    float _pad;
};

class Material : public MaterialBase {
public:
    Material(const char* aMaterialName = "Default") : mMaterialName(aMaterialName){}
    virtual void Create(MaterialBase *aBaseMaterial);
    virtual void CreateProperties(uint32_t aBinding, const MaterialProperties &aMaterialProperties);
    virtual void BindTexture(const std::vector<VkDescriptorImageInfo> &textureInfo, const uint8_t aBinding) const;

    void BindTexture(const VkDescriptorImageInfo &imageInfo, uint8_t aBinding) const;

    virtual void AddBinding(uint32_t aBinding, uint32_t aCount,
                            VkDescriptorType aType, VkShaderStageFlagBits aShaderStage);
    virtual void SetBuffers(const AllocatedBuffer &aBuffer, const uint8_t aBinding, const uint8_t aIndex) const;

    VkDescriptorSet GetDescriptorSet() const { return mDescriptorSet; }
    VkDescriptorSetLayout GetDescriptorLayout() const { return mLayout; }

    void Destroy() override;

    MaterialProperties mMaterialProperties;
    AllocatedBuffer mPropertiesBuffer;
    const char * mMaterialName;

protected:
    MaterialBase *mMaterialBase;
    VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet;

    std::unordered_map<uint8_t, VkWriteDescriptorSet> mBoundTextures;
};
