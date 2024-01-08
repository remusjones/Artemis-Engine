//
// Created by Remus on 5/01/2024.
//

#include "Cubemap.h"

#include "LoadUtilities.h"
#include "Logger.h"
#include "File Management/FileManagement.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void Cubemap::Create(MaterialBase *aBaseMaterial, const char *aMaterialName) {
    AllocatedImage image;

    const int mipLevel = 1;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    // Load n sides of cubemap here ..

    // TODO: Pass these into the constructor
    std::vector<std::string> cubemapImagePaths;

    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunRight.png");
    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunLeft.png");
    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunTop.png");
    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunBottom.png");
    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunFront.png");
    cubemapImagePaths.push_back(FileManagement::GetWorkingDirectory() + "/Assets/Textures/MegaSunBack.png");

    LoadUtilities::LoadImagesFromDisk(gGraphics, cubemapImagePaths, image, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);

    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;

    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerCreateInfo.mipLodBias = 0.0f;
    samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.minLod = 0.0f;
    samplerCreateInfo.maxLod = static_cast<float>(mipLevel);
    samplerCreateInfo.maxAnisotropy = 4.0;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE; {
        auto result = vkCreateSampler(gGraphics->mLogicalDevice, &samplerCreateInfo, nullptr, &mSampler);
        if (result != VK_SUCCESS) {
            Logger::Log(spdlog::level::critical, "failed to create Sampler!");
        }
    }

    VkImageViewCreateInfo view{};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    view.format = format;
    view.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 6;
    view.subresourceRange.levelCount = mipLevel;
    view.image = image.mImage; {
        auto result = vkCreateImageView(gGraphics->mLogicalDevice, &view, nullptr, &mImageView);
        if (result != VK_SUCCESS) {
            Logger::Log(spdlog::level::critical, "failed to create image view!");
        }
    }

    mDescriptorImageInfo.sampler = mSampler;
    mDescriptorImageInfo.imageView = mImageView;
    mDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    AddBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
    AddBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    Material::Create(this, aMaterialName);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mSceneBuffer, 0, 0);

    std::vector<VkDescriptorImageInfo> descriptorInfos;
    descriptorInfos.push_back(mDescriptorImageInfo);
    BindTexture(descriptorInfos, 1);
}

void Cubemap::Destroy() {
    Material::Destroy();
    vkDestroyImageView(gGraphics->mLogicalDevice, mImageView, nullptr);
    vkDestroySampler(gGraphics->mLogicalDevice, mSampler, nullptr);
    vmaDestroyImage(gGraphics->mAllocator, mAllocatedImage.mImage, mAllocatedImage.mAllocation);
}
