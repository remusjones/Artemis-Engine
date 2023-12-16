//
// Created by Remus on 7/11/2021.
//

#ifndef INC_3DENGINE_VULKANSYSTEMSTRUCTS_H
#define INC_3DENGINE_VULKANSYSTEMSTRUCTS_H

#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> mGraphicsFamily;
    std::optional<uint32_t> mPresentFamily;

    bool isComplete()
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

#endif //INC_3DENGINE_VULKANSYSTEMSTRUCTS_H

