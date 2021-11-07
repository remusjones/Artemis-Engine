//
// Created by Remus on 7/11/2021.
//

#ifndef INC_3DENGINE_SYSTEMSTRUCTS_H
#define INC_3DENGINE_SYSTEMSTRUCTS_H


struct QueueFamilyIndices
{
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;

    bool isComplete()
    {
        return m_graphicsFamily.has_value() && m_presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_presentModes;
};

#endif //INC_3DENGINE_SYSTEMSTRUCTS_H

