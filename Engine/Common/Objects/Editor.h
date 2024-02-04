//
// Created by Remus on 3/02/2024.
//

#pragma once
#include "ImGuiLayer.h"

struct FileInfo {
    std::string fileName;
};

class Editor : ImGuiLayer{
public:
    void OnImGuiRender() override;

private:

    // Draws Directory Content
    void DrawContent();

    // Refreshes the directory content
    void RefreshDirectoryContent();

    // Create map of directory
    std::unordered_map<std::string, FileInfo> mDirectoryContent;
};
