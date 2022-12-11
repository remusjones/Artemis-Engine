//
// Created by Remus on 9/12/2022.
//

#include <fstream>
#include "rem_fileManagement.h"
#include <minwindef.h>
#include <libloaderapi.h>

std::vector<char> rem_fileManagement::GetShaderFileData(const std::string &filename)
{
    std::string finalDirectory = GetCurrentDirectory();
    finalDirectory.append(SHADER_DIRECTORY);
    finalDirectory.append(filename);

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

std::string rem_fileManagement::GetCurrentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}