#include "Texture.h"
#include <stdexcept>
#include "LoadUtilities.h"
#include <Logger.h>
#include "VulkanGraphicsImpl.h"
#include "Vulkan/Helpers/VulkanInitialization.h"


void Texture::LoadImagesFromDisk(const std::vector<std::string> &aPaths) {
    LoadUtilities::LoadImagesFromDisk(gGraphics, aPaths, mAllocatedImage);
    mImageCount = aPaths.size();
}

void Texture::Create(VkFilter aSamplerFilter,
                     VkSamplerAddressMode aSamplerAddressMode) {
    const VkSamplerCreateInfo samplerInfo = VulkanInitialization::SamplerCreateInfo(
        aSamplerFilter, aSamplerAddressMode);
    vkCreateSampler(gGraphics->mLogicalDevice, &samplerInfo, nullptr, &mSampler);

    // Creating image views for each layer
    for (uint32_t i = 0; i < mImageCount; ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = mAllocatedImage.mImage; // the VkImage object
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = mImageCount;

        VkImageView imageView;
        if (vkCreateImageView(gGraphics->mLogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
        mImageViews.push_back(imageView); // add to image view array
        VkDescriptorImageInfo imageBufferInfo;
        imageBufferInfo.sampler = mSampler;
        imageBufferInfo.imageView = imageView;
        imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        mImageBufferInfo.push_back(imageBufferInfo); // add image info to the array
    }
}

void Texture::Destroy() const {
    vmaDestroyImage(gGraphics->mAllocator, mAllocatedImage.mImage, mAllocatedImage.mAllocation);
    // Destroy each imageView
    for (const auto imageView: mImageViews) {
        vkDestroyImageView(gGraphics->mLogicalDevice, imageView, nullptr);
    }
    vkDestroySampler(gGraphics->mLogicalDevice, mSampler, nullptr);
}
