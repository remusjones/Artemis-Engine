//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vulkan/vulkan_core.h>
#include "MaterialBase.h"

class Buffer;
class Material : public MaterialBase {
public:
    void Create(const MaterialBase* aBaseMaterial, const char* aMaterialName);
    void SetBuffer(const Buffer* aBuffer);

private:
    const MaterialBase* mMaterialBase;
    VkDescriptorSet nDescriptorSet = VK_NULL_HANDLE;
};
