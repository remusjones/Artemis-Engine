//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "../../Base/Common/Buffers/AllocatedImage.h"

class VulkanInitialization {
    VulkanInitialization() = default;
public:
    static VkImageCreateInfo CreateImageInfo(const VkFormat aFormat, const VkImageUsageFlags aUsageFlags, const VkExtent3D aExtent) {

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.imageType = VK_IMAGE_TYPE_2D;

        info.format = aFormat;
        info.extent = aExtent;

        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = aUsageFlags;

        return info;
    }
    static VkImageViewCreateInfo ImageViewCreateInfo(const VkFormat aFormat,
                                                     const VkImage aImage, const VkImageAspectFlags aAspectFlags) {
	    //build a image-view for the depth image to use for rendering
    	VkImageViewCreateInfo info = {};
    	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    	info.pNext = nullptr;

    	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    	info.image = aImage;
    	info.format = aFormat;
    	info.subresourceRange.baseMipLevel = 0;
    	info.subresourceRange.levelCount = 1;
    	info.subresourceRange.baseArrayLayer = 0;
    	info.subresourceRange.layerCount = 1;
    	info.subresourceRange.aspectMask = aAspectFlags;

    	return info;
    }

    static VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(
	    const bool aDepthTest, const bool aDepthWrite, const VkCompareOp aCompareOp) {

        VkPipelineDepthStencilStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;

        info.depthTestEnable = aDepthTest ? VK_TRUE : VK_FALSE;
        info.depthWriteEnable = aDepthWrite ? VK_TRUE : VK_FALSE;
        info.depthCompareOp = aDepthTest ? aCompareOp : VK_COMPARE_OP_ALWAYS;
        info.depthBoundsTestEnable = VK_FALSE;
        info.minDepthBounds = 0.0f; // Optional
        info.maxDepthBounds = 1.0f; // Optional
        info.stencilTestEnable = VK_FALSE;

        return info;
    }
	static VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags /*= 0*/)
    {
    	VkCommandBufferBeginInfo info = {};
    	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    	info.pNext = nullptr;

    	info.pInheritanceInfo = nullptr;
    	info.flags = flags;
    	return info;
    }
	static VkWriteDescriptorSet WriteDescriptorImage(VkDescriptorType type,
		VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding)
    {
    	VkWriteDescriptorSet write = {};
    	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    	write.pNext = nullptr;

    	write.dstBinding = binding;
    	write.dstSet = dstSet;
    	write.descriptorCount = 1;
    	write.descriptorType = type;
    	write.pImageInfo = imageInfo;

    	return write;
    }
	static VkSamplerCreateInfo SamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT)
	{
		VkSamplerCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext = nullptr;

		info.magFilter = filters;
		info.minFilter = filters;
		info.addressModeU = samplerAdressMode;
		info.addressModeV = samplerAdressMode;
		info.addressModeW = samplerAdressMode;

		return info;
	}
	static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count /*= 1*/,
	VkCommandBufferLevel level /*=
	VK_COMMAND_BUFFER_LEVEL_PRIMARY*/)
    {
    	VkCommandBufferAllocateInfo info = {};
    	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    	info.pNext = nullptr;

    	info.commandPool = pool;
    	info.commandBufferCount = count;
    	info.level = level;
    	return info;
    }

	static VkSubmitInfo SubmitInfo(VkCommandBuffer* cmd)
    {
    	VkSubmitInfo info = {};
    	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    	info.pNext = nullptr;

    	info.waitSemaphoreCount = 0;
    	info.pWaitSemaphores = nullptr;
    	info.pWaitDstStageMask = nullptr;
    	info.commandBufferCount = 1;
    	info.pCommandBuffers = cmd;
    	info.signalSemaphoreCount = 0;
    	info.pSignalSemaphores = nullptr;

    	return info;
    }

};
