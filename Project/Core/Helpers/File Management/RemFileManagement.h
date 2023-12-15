//
// Created by Remus on 9/12/2022.
//


#pragma once
#include <vector>
#include <string>
#include "FileManagementInfo.h"


class RemFileManagement {
private:
    RemFileManagement() = default;
public:

    static std::vector<char> GetShaderFileData(const std::string& filename);
    static void SetShaderInfoSettings(const FileManagementShaderInfo&
    shaderInfo);
    static const FileManagementShaderInfo& GetShaderInfoSettings();

    static std::string GetCurrentDirectory();
private:
    static inline FileManagementShaderInfo ShaderFileSettings;
};


