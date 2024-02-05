//
// Created by Remus on 9/12/2022.
//


#pragma once
#include <vector>
#include <string>
#include "FileManagementInfo.h"

class FileManagement {
public:
    FileManagement() = delete;

    static std::vector<char> GetShaderFileData(const std::string &filename);

    static std::vector<char> GetShaderFileDataPath(const char *aRelativeDirectory);

    static void SetFileInformationSettings(const FileManagementSettings &
        shaderInfo);

    static const FileManagementSettings &GetInfoSettings();

    static std::string GetWorkingDirectory();

    static std::string MakeAssetPath(const std::string &aRelativeFilePath);

    static std::string GetAssetPath();

    static bool FileExists(const std::string &aAbsolutePath);

private:
    static inline FileManagementSettings sFileInformationSettings;
};
