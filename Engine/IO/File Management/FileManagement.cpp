//
// Created by Remus on 9/12/2022.
//

#include <fstream>
#include "FileManagement.h"
#include <minwindef.h>
#include <libloaderapi.h>

std::vector<char> FileManagement::GetShaderFileData(const std::string &filename)
{
    std::string finalDirectory = GetCurrentDirectory();
    finalDirectory.append(ShaderFileSettings.pShaderDirectory);
    finalDirectory.append(filename);

    std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file " + filename);
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::string FileManagement::GetCurrentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

void FileManagement::SetShaderInfoSettings(
        const FileManagementShaderInfo &shaderInfo) {
    ShaderFileSettings = shaderInfo;
}

const FileManagementShaderInfo &FileManagement::GetShaderInfoSettings() {
    return ShaderFileSettings;
}

std::vector<char> FileManagement::GetShaderFileDataPath(const char* aRelativeDirectory) {
    std::string finalDirectory = GetCurrentDirectory();
    finalDirectory.append(aRelativeDirectory);

    std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(),fileSize);
    file.close();
    return buffer;
}
