//
// Created by Remus on 30/12/2021.
//

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef INC_3DENGINE_SHADERCOMPONENT_H
#define INC_3DENGINE_SHADERCOMPONENT_H

#include <vector>
#include <string>
#include <map>
#include "SystemStructs.h"



enum ShaderType
{
    NONE,
    FRAGMENT,
    VERTEX,
    BOTH
};

class ShaderComponent {
public:
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};

    VkShaderModule  m_fragment;
    VkShaderModule  m_vertex;

    ShaderComponent(VkShaderModule& frag,VkShaderModule& vert)
    {
        m_fragment = frag;
        m_vertex = vert;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = m_vertex;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = m_fragment;
        fragShaderStageInfo.pName = "main";

        m_shaderStages.push_back(vertShaderStageInfo);
        m_shaderStages.push_back(fragShaderStageInfo);
    }
    ShaderComponent(VkShaderModule& target, ShaderType shaderType)
    {

        if (shaderType == FRAGMENT)
        {
            m_fragment = target;

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = m_fragment;
            fragShaderStageInfo.pName = "main";

            m_shaderStages.push_back(fragShaderStageInfo);

        }
        else if (shaderType == VERTEX)
        {

            m_vertex = target;

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = m_vertex;
            vertShaderStageInfo.pName = "main";

            m_shaderStages.push_back(vertShaderStageInfo);
        }
    }
};


#endif //INC_3DENGINE_SHADERCOMPONENT_H
