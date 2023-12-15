//
// Created by Remus on 30/12/2021.
//

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#ifndef INC_3DENGINE_SHADERCOMPONENT_H
#define INC_3DENGINE_SHADERCOMPONENT_H

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include "SystemStructs.h"
#include "Systems/Rendering/Data/Vertex.h"

class RemShaderComponent {
public:
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};

    const char* m_shaderName;
    VkShaderModule  m_fragment;
    VkShaderModule  m_vertex;

    RemShaderComponent() = default;
    RemShaderComponent(VkShaderModule& frag,
                       VkShaderModule& vert,
                       const char* shaderName = "default"
                       )
    {
        m_shaderName = shaderName;
        m_fragment = frag;
        m_vertex = vert;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = m_vertex;
        vertShaderStageInfo.pName = m_shaderName;

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = m_fragment;
        fragShaderStageInfo.pName = m_shaderName;

        m_shaderStages.push_back(vertShaderStageInfo);
        m_shaderStages.push_back(fragShaderStageInfo);
    }

};


#endif //INC_3DENGINE_SHADERCOMPONENT_H
