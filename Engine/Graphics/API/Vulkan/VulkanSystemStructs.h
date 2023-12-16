//
// Created by Remus on 7/11/2021.
//
#pragma once
#include <optional>
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <vector>

struct QueueFamilyIndices
{
    std::optional<uint32_t> mGraphicsFamily;
    std::optional<uint32_t> mPresentFamily;

    bool IsComplete() const
    {
        return mGraphicsFamily.has_value() && mPresentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR mCapabilities;
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR> mPresentModes;
};