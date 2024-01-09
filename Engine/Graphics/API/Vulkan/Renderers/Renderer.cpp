//
// Created by Remus on 9/01/2024.
//

#include "Renderer.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"

void Renderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    void *data;
    // TODO: Move the properties binding out of here? lol
    if (mMaterial->mPropertiesBuffer.mAllocation != nullptr) {
        vmaMapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation, &data);
        memcpy(data, &mMaterial->mMaterialProperties, sizeof(MaterialProperties));
        vmaUnmapMemory(gGraphics->mAllocator, mMaterial->mPropertiesBuffer.mAllocation);
    }
}

void Renderer::BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) {
}

void Renderer::DestroyRenderer() {
}
