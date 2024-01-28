//
// Created by Remus on 9/01/2024.
//

#include "Renderer.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"

void Renderer::Render(VkCommandBuffer aCommandBuffer, const Scene &aScene) {
    void *data;
    if (mMaterial->mPropertiesBuffer.IsAllocted()) {
        AllocatedBuffer::MapMemory(
            gGraphics->mAllocator, &mMaterial->mMaterialProperties, mMaterial->mPropertiesBuffer.GetAllocation(),
            sizeof(MaterialProperties));
    }
}

void Renderer::BindRenderer(GraphicsPipeline &aBoundGraphicsPipeline) {
}

void Renderer::DestroyRenderer() {
}
