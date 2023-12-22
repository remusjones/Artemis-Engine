//
// Created by Remus on 9/12/2022.
//


#pragma once
#include <vector>
#include <string>
#include "FileManagementInfo.h"

// TODO: Make less specific to Vulkan
class FileManagement {
private:
    FileManagement() = default;
public:

    static std::vector<char> GetShaderFileData(const std::string& filename);
    static std::vector<char> GetShaderFileDataPath(const char* aRelativeDirectory);
    static void SetShaderInfoSettings(const FileManagementShaderInfo&
    shaderInfo);
    static const FileManagementShaderInfo& GetShaderInfoSettings();

    static std::string GetWorkingDirectory();
private:
    static inline FileManagementShaderInfo ShaderFileSettings;
};


