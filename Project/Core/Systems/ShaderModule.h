//
// Created by Remus on 6/11/2021.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef INC_3DENGINE_SHADERMODULE_H
#define INC_3DENGINE_SHADERMODULE_H

#include <vector>
#include <string>
#include <map>
#include "SystemStructs.h"

// Helpers
#define SHADER_VERTEX_SUFFIX "_v"
#define SHADER_FRAGMENT_SUFFIX "_f"
#define SHADER_FILE_EXTENSION ".spv"
#define SHADER_DIRECTORY "\\Shaders\\"


//TODO Rename ShaderModule to RenderPipeline

enum ShaderType
{
    NONE,
    FRAGMENT,
    VERTEX,
    BOTH
};

struct Shader
{
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};

    VkShaderModule  m_fragment;
    VkShaderModule  m_vertex;
    ShaderType m_type = NONE;

    Shader(VkShaderModule& frag,VkShaderModule& vert)
    {
        m_fragment = frag;
        m_vertex = vert;
        m_type = BOTH;

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
    Shader(VkShaderModule& target, ShaderType shaderType)
    {
        m_type = shaderType;

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
    void DestroyShader(VkDevice& logicalDevice)
    {
        if (m_fragment != nullptr)
            vkDestroyShaderModule(logicalDevice, m_fragment, nullptr);
        if (m_vertex != nullptr)
            vkDestroyShaderModule(logicalDevice, m_vertex, nullptr);
    }
};



class ShaderModule
{
public:

    void Initialize(VkDevice& logicalDevice,
                    VkSwapchainKHR& swapChainKhr,
                    VkExtent2D& swapChainExtent,
                    VkFormat& swapChainImageFormat,
                    std::vector<VkImageView>& swapChainImageViews,
                    VkPhysicalDevice& physicalDevice,
                    VkQueue& graphicsQueue,
                    VkQueue& presentQueue
                    );

    VkShaderModule CreateShaderModule(const std::vector<char>& code);

    VkResult LoadShader(const std::string& shaderName, ShaderType shaderType);
    void CreatePipelineLayout();
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateCommandPool(QueueFamilyIndices& queueFamilyIndices);
    void CreateCommandBuffer();
    void UnloadShader(const std::string& shaderName);
    void DrawFrame();
    void CreateSemaphores();

    void Cleanup();
    static std::vector<char> ReadFile(const std::string& filename);


private:
    std::vector<Shader*> m_loadedShaders{};
    VkDevice m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkSwapchainKHR m_swapChain;
    VkExtent2D m_swapChainExtent;
    VkFormat m_swapChainImageFormat;
    VkPipeline m_graphicsPipeline;
    std::vector<VkImageView> m_swapChainImageViews{};
    std::vector<VkFramebuffer> m_swapChainFrameBuffers{};
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers{};
    bool m_hasCreatedPipeline = false;

    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;


};


#endif //INC_3DENGINE_SHADERMODULE_H
