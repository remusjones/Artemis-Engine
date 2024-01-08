//
// Created by Remus on 17/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Objects/ImGuiLayer.h"

class MaterialBase : public ImGuiLayer{
public:
    virtual void OnImGuiRender() {};
    virtual void Destroy();
    void AddBinding(const uint32_t aBinding, const uint32_t aCount, const VkDescriptorType aType, const
    VkShaderStageFlagBits aStage);

    void Create();

    std::vector<VkDescriptorSetLayoutBinding> mBindings;

    VkDescriptorSetLayout mLayout;
};
