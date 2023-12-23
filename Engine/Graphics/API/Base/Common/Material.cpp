//
// Created by Remus on 17/12/2023.
//

#include "Material.h"

#include "VulkanGraphicsImpl.h"

void Material::Create(const MaterialBase *aBaseMaterial, const char *aMaterialName) {
    mMaterialBase = aBaseMaterial;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.bindingCount = mBindings.size();
    createInfo.pBindings = mBindings.data();
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    vkCreateDescriptorSetLayout(gGraphics->mLogicalDevice, &createInfo,
    nullptr, &mLayout);
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

void Material::Destroy() {
    vkDestroyDescriptorSetLayout(gGraphics->mLogicalDevice, mLayout, nullptr);
   // vkFreeDescriptorSets(gGraphics->mLogicalDevice, gGraphics->mDescriptorPool, 1, nullptr);
}
