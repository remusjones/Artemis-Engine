//
// Created by Remus on 3/02/2024.
//

#pragma once
#include <memory>

#include "ImGuiLayer.h"
#include "DirectoryMonitor.h"


class Editor : ImGuiLayer{
public:
    void Create();
    void OnImGuiRender() override;
private:
    // Draws Directory Content
    void DrawContent();

    std::unique_ptr<DirectoryMonitor> mAssetDirectoryMonitor;
};
