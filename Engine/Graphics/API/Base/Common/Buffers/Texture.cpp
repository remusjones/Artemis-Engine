#include "Texture.h"

#include "VulkanGraphicsImpl.h"
#include "Vulkan/Helpers/VulkanInitialization.h"


void Texture::Create(VkFilter aSamplerFilter,
                     VkSamplerAddressMode aSamplerAddressMode) {
    VkImageViewCreateInfo imageinfo = VulkanInitialization::ImageViewCreateInfo(
        VK_FORMAT_R8G8B8A8_SRGB, mAllocatedImage.mImage,
        VK_IMAGE_ASPECT_COLOR_BIT);

    vkCreateImageView(gGraphics->mLogicalDevice, &imageinfo, nullptr, &mImageView);

    const VkSamplerCreateInfo samplerInfo = VulkanInitialization::SamplerCreateInfo(
        aSamplerFilter, aSamplerAddressMode);
    vkCreateSampler(gGraphics->mLogicalDevice, &samplerInfo, nullptr, &mSampler);

    mImageBufferInfo.sampler = mSampler;
    mImageBufferInfo.imageView = mImageView;
    mImageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Texture::Destroy() const {
    vmaDestroyImage(gGraphics->mAllocator, mAllocatedImage.mImage, mAllocatedImage.mAllocation);
    vkDestroyImageView(gGraphics->mLogicalDevice, mImageView, nullptr);
    vkDestroySampler(gGraphics->mLogicalDevice, mSampler, nullptr);
}
