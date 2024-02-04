//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"

#include <filesystem>

#include "Logger.h"
#include "VulkanGraphicsImpl.h"
#include "File Management/FileManagement.h"
#include "Scenes/Scene.h"

void Editor::OnImGuiRender() {
    //DrawContent();
    gGraphics->mActiveScene->OnImGuiRender();
}

void Editor::DrawContent() {
    std::filesystem::path path = FileManagement::GetWorkingDirectory();
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        Logger::Log(entry.path().string());
    }
}
