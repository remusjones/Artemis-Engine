//
// Created by Remus on 6/11/2021.
//
#include <fstream>
#include "ShaderModule.h"
#include <Windows.h>

void ShaderModule::Initialize(VkDevice& logicalDevice)
{
    // Register the shader module
    m_logicalDevice = logicalDevice;
}


void ShaderModule::Cleanup()
{
    // release all loaded shaders
    while(m_loadedShaders.size() > 0)
    {
        m_loadedShaders[m_loadedShaders.size() -1]->DestroyShader(m_logicalDevice);
        delete m_loadedShaders[m_loadedShaders.size() - 1];
        m_loadedShaders.pop_back();
    }
}

void ShaderModule::UnloadShader(const std::string& shaderName)
{
    // unload a specific shader
}

VkResult ShaderModule::LoadShader(const std::string& shaderName, ShaderType shaderType)
{
    if (shaderType == NONE)
        throw std::runtime_error("Cannot create a Shader with the ShaderType of NONE");


    if (shaderType == BOTH)
    {
        // shaderName and apply suffix targets and populate shader
        std::string vertex = shaderName;
        vertex.append(SHADER_VERTEX_SUFFIX);
        vertex.append(SHADER_FILE_EXTENSION);

        std::string frag = shaderName;
        frag.append(SHADER_FRAGMENT_SUFFIX);
        frag.append(SHADER_FILE_EXTENSION);

        auto fragData = CreateShaderModule(ReadFile(frag));
        auto vertData = CreateShaderModule(ReadFile(vertex));

        Shader* shader = new Shader(fragData, vertData);

        m_loadedShaders.push_back(shader);


        return VK_SUCCESS;

    }else if (shaderType == VERTEX || shaderType == FRAGMENT)
    {
        auto t = CreateShaderModule(ReadFile(shaderName));

        Shader* shader = new Shader(t, shaderType);
        m_loadedShaders.push_back(shader);
        return VK_SUCCESS;
    }

    return VK_ERROR_INVALID_SHADER_NV;

}
std::string GetCurrentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}
std::vector<char> ShaderModule::ReadFile(const std::string &filename)
{
    std::string finalDirectory = GetCurrentDirectory();
    finalDirectory.append(SHADER_DIRECTORY);
    finalDirectory.append(filename);

//test
    std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule ShaderModule::CreateShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;

}

