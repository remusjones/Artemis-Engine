//
// Created by Remus on 17/12/2023.
//

#include "Material.h"

#include <stdexcept>

#include "VulkanGraphicsImpl.h"
#include "Buffers/Texture.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void Material::Create(MaterialBase *aBaseMaterial, const char *aMaterialName) {

    mMaterialBase = aBaseMaterial;
    const VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(mBindings.size()),
        .pBindings = mBindings.data(),
    };
    vkCreateDescriptorSetLayout(gGraphics->mLogicalDevice, &createInfo, nullptr, &mLayout);

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = gGraphics->mVulkanEngine.mDescriptorPool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &mLayout;
    VkResult err = vkAllocateDescriptorSets(gGraphics->mLogicalDevice, &alloc_info, &mDescriptorSet);
    if (err == VK_ERROR_OUT_OF_POOL_MEMORY) {
        throw std::runtime_error("Out of pool memory");
    }

    for (auto boundTexture: mBoundTextures) {
    }
}

void Material::CreateProperties(const uint32_t aBinding, const MaterialProperties &aMaterialProperties) {
    mMaterialProperties = aMaterialProperties;
    mPropertiesBuffer = gGraphics->mVulkanEngine.CreateBuffer(sizeof(MaterialProperties),
                                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                              VMA_MEMORY_USAGE_CPU_TO_GPU);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        SetBuffers(mPropertiesBuffer, aBinding, 0);
}

void Material::BindTexture(Texture &aTexture, const uint8_t aBinding) {
    mBoundTextures[aBinding] = VulkanInitialization::WriteDescriptorImage(
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GetDescriptorSet(), &aTexture.mImageBufferInfo, aBinding);
    vkUpdateDescriptorSets(gGraphics->mLogicalDevice, 1, &mBoundTextures[aBinding], 0, nullptr);
}

void Material::AddBinding(const uint32_t aBinding, const uint32_t aCount,
                          const VkDescriptorType aType, VkShaderStageFlagBits aShaderStage) {
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = aBinding;
    binding.descriptorCount = aCount;
    binding.stageFlags = aShaderStage;
    binding.descriptorType = aType;
    mBindings.push_back(binding);
}

void Material::SetBuffers(const AllocatedBuffer &aBuffer, const uint8_t aBinding, const uint8_t aIndex) {
    const VkDescriptorSetLayoutBinding &binding = mBindings[aBinding];

    auto *descriptors = static_cast<VkDescriptorBufferInfo *>(alloca(
        sizeof(VkDescriptorBufferInfo) * binding.descriptorCount));
    for (int i = 0; i < binding.descriptorCount; i++) {
        descriptors[i].buffer = aBuffer.mBuffer;
        descriptors[i].offset = 0;
        descriptors[i].range = VK_WHOLE_SIZE;
    }

    VkWriteDescriptorSet writeDescription = {};
    writeDescription.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescription.dstSet = mDescriptorSet;
    writeDescription.dstBinding = aBinding;
    writeDescription.descriptorCount = binding.descriptorCount;

    writeDescription.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescription.pBufferInfo = descriptors;

    vkUpdateDescriptorSets(gGraphics->mLogicalDevice, 1, &writeDescription, 0, nullptr);
}

void Material::Destroy() {
    vkDestroyDescriptorSetLayout(gGraphics->mLogicalDevice, mLayout, nullptr);
    vkFreeDescriptorSets(gGraphics->mLogicalDevice, gGraphics->mVulkanEngine.mDescriptorPool, 1, &mDescriptorSet);

    if (mPropertiesBuffer.mBuffer)
        mPropertiesBuffer.Destroy();
}
