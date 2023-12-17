//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

class MaterialBase {
public:

    void AddBinding(const uint32_t aBinding, const uint32_t aCount, const VkDescriptorType aType, const
    VkShaderStageFlagBits aStage);
    void Create();

    std::vector<VkDescriptorSetLayoutBinding> mBindings;

    VkDescriptorSetLayout mLayout;
};
