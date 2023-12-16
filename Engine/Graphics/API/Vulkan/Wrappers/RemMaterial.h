//
// Created by Remus on 30/12/2021.
//
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "SystemStructs.h"

class RemMaterial {
public:
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};

    VkShaderModule  m_fragment;
    VkShaderModule  m_vertex;

    RemMaterial() = default;
    RemMaterial(VkShaderModule& frag,
                VkShaderModule& vert,
                const char* shaderEntryName = "main")
    {
        m_fragment = frag;
        m_vertex = vert;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = m_vertex;
        vertShaderStageInfo.pName = shaderEntryName;

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = m_fragment;
        fragShaderStageInfo.pName = shaderEntryName;

        m_shaderStages.push_back(vertShaderStageInfo);
        m_shaderStages.push_back(fragShaderStageInfo);
    }


    void CleanupShaderModules(VkDevice device)
    {
        if (m_vertex != nullptr)
            vkDestroyShaderModule(device, m_vertex, nullptr);
        if (m_fragment != nullptr)
            vkDestroyShaderModule(device, m_fragment, nullptr);
    }

};


