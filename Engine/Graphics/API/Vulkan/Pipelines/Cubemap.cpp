//
// Created by Remus on 5/01/2024.
//

#include "Cubemap.h"

#include "LoadUtilities.h"
#include "File Management/FileManagement.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void Cubemap::Create() {
    AllocatedImage image;

    const int mipLevel = 1;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

    // Load n sides of cubemap here ..
    LoadUtilities::LoadCubemap(gGraphics, std::string(FileManagement::GetWorkingDirectory() +
                                                      "/Assets/Textures/Stone Wall.png").c_str(), format, image);
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    //if (m_NearestFilter)
    {
        samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
        samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
    }
    //else
    //{
    //    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    //    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    //}
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
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    auto result = vkCreateSampler(gGraphics->mLogicalDevice, &samplerCreateInfo, nullptr, &mSampler);

    VkImageViewCreateInfo view {};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    view.format = format;
    view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount = 1;
    view.subresourceRange.levelCount = mipLevel;
    view.image = image.mImage;



    // TODO: Create imagview and descriptor
    //{
    //    auto result = vkCreateImageView(device, &view, nullptr, &m_ImageView);
    //    if (result != VK_SUCCESS)
    //    {
    //        LOG_CORE_CRITICAL("failed to create image view!");
    //    }
    //}
//
    //mDescriptorImageInfo.sampler     = m_Sampler;
    //mDescriptorImageInfo.imageView   = m_ImageView;
    //mDescriptorImageInfo.imageLayout = m_ImageLayout;

}

