//
// Created by Remus on 9/12/2022.
//

#include <fstream>
#include <cstring> // New include to support C file operations
#include "FileManagement.h"

#include <io.h>
#include <libloaderapi.h>


std::vector<char> FileManagement::GetShaderFileData(const std::string &filename)
{
    std::string finalDirectory = GetWorkingDirectory();
    finalDirectory.append(sFileInformationSettings.pShaderDirectory);
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
std::string FileManagement::GetWorkingDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
std::string FileManagement::MakeAssetPath(const std::string &aRelativeFilePath) {
    std::string basePath = GetAssetPath();
    basePath.append(aRelativeFilePath);
    return basePath;
}
std::string FileManagement::GetAssetPath(){
    std::string path = GetWorkingDirectory();
    path.append(sFileInformationSettings.pAssetPath);
    return path;
}

bool FileManagement::FileExists(const std::string &aAbsolutePath) {
    return access( aAbsolutePath.c_str(), 0 ) == 0;
}

void FileManagement::SetFileInformationSettings(
        const FileManagementSettings &shaderInfo) {
    sFileInformationSettings = shaderInfo;
}

const FileManagementSettings &FileManagement::GetInfoSettings() {
    return sFileInformationSettings;
}

std::vector<char> FileManagement::GetShaderFileDataPath(const char* aRelativeDirectory) {
    std::string finalDirectory = GetWorkingDirectory();
    finalDirectory.append(aRelativeDirectory);

    std::ifstream file(finalDirectory, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file" + std::string(aRelativeDirectory));
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}