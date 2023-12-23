//
// Created by Remus on 17/12/2023.
//

#include "Material.h"

#include <stdexcept>

#include "VulkanGraphicsImpl.h"

void Material::Create(const MaterialBase *aBaseMaterial, const char *aMaterialName) {
    mMaterialBase = aBaseMaterial;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindingCount = mBindings.size();
    createInfo.pBindings = mBindings.data();
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    vkCreateDescriptorSetLayout(gGraphics->mLogicalDevice, &createInfo,
                                nullptr, &mLayout);


    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = gGraphics->mDescriptorPool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &mLayout;
    VkResult err = vkAllocateDescriptorSets(gGraphics->mLogicalDevice, &alloc_info, &mSet);
    if (err == VK_ERROR_OUT_OF_POOL_MEMORY) {
        throw std::runtime_error("Out of pool memory");
    }
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
    const VkDescriptorSetLayoutBinding& binding = mBindings[aBinding];

    auto* descriptors = static_cast<VkDescriptorBufferInfo *>(alloca(sizeof(VkDescriptorBufferInfo) * binding.descriptorCount));
    for(int i = 0; i < binding.descriptorCount; i++) {
        descriptors[i].buffer = aBuffer.mBuffer;
        descriptors[i].offset = 0;
        descriptors[i].range = VK_WHOLE_SIZE;
    }

    VkWriteDescriptorSet writeDescription = {};
    writeDescription.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescription.dstSet = mSet;
    writeDescription.dstBinding = aBinding;
    writeDescription.descriptorCount = binding.descriptorCount;

    writeDescription.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescription.pBufferInfo = descriptors;

    vkUpdateDescriptorSets(gGraphics->mLogicalDevice, 1, &writeDescription, 0, nullptr);
}

void Material::Destroy() {
    vkDestroyDescriptorSetLayout(gGraphics->mLogicalDevice, mLayout, nullptr);
    vkFreeDescriptorSets(gGraphics->mLogicalDevice, gGraphics->mDescriptorPool, 1, nullptr);
}
