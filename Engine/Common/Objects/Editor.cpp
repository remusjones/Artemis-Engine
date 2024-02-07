//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"
#include "ProjectExplorer/DirectoryMonitor.h"
#include "FileManagement.h"
#include "imgui.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::Create(){
    mAssetDirectoryMonitor = std::make_unique<DirectoryMonitor>();
    mAssetDirectoryMonitor->CreateDirectoryMonitor(FileManagement::GetAssetPath());

    // Works around the path having a '/' at the end TODO: fix this
    mCurrentPath = std::filesystem::path(FileManagement::GetAssetPath()).parent_path();
    mContextBounds = std::filesystem::path(FileManagement::GetAssetPath()).parent_path();
}

void Editor::OnImGuiRender() {
    //DrawContent();
    gGraphics->mActiveScene->OnImGuiRender();
    DrawContent();
}

void Editor::FileBrowser(const std::filesystem::path& aPath)
{
    ImGui::Text("Current path: %s", aPath.string().c_str());
    if (aPath.has_parent_path() && aPath != mContextBounds)
    {
        if (ImGui::Button("Up"))
        {
            mCurrentPath = aPath.parent_path();
            GetFilesInDirectory(mCurrentPath, mFilesInDirectory);
        }
    }
    if (is_directory(aPath))
    {
        for (auto& entry : std::filesystem::directory_iterator(aPath))
        {
            ImGui::PushID(entry.path().string().c_str());
            if (is_directory(entry.status()))
            {
                if (ImGui::Button(entry.path().filename().string().c_str()))
                {
                    mCurrentPath = entry.path();
                    GetFilesInDirectory(mCurrentPath, mFilesInDirectory);
                }
            }
            ImGui::PopID();
        }
    }
}

void Editor::DrawContent() {

    ImGui::Begin("DirectoryBrowser");
    FileBrowser(mCurrentPath);
    ImGui::End();

    ImGui::Begin("File Display");

    if (ImGui::Button(GetUniqueLabel("Refresh Files"))) {
        mAssetDirectoryMonitor->UpdateDirectoryMonitor();
    }

    ImGui::BeginTable("files", 2, ImGuiTableFlags_BordersOuterH |
    ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV);

    for (const auto& entry: mFilesInDirectory) {
        // TODO: Extract directly from importer files instead of raw paths

        ImGui::Button((std::string("##") + entry.path().filename().string()).c_str(),
            ImVec2(100,100));
        ImGui::Text("%s", entry.path().filename().string().c_str());
        ImGui::TableNextColumn();
    }
    ImGui::EndTable();
    mAssetDirectoryMonitor->OnImGuiRender();
    ImGui::End();
}

void Editor::GetFilesInDirectory(const std::filesystem::path &aPath, std::vector<std::filesystem::directory_entry> &aFilesInDirectory) {
    aFilesInDirectory.clear();
    for (auto& entry : std::filesystem::directory_iterator(aPath)) {
        if (!is_directory(entry))
            aFilesInDirectory.push_back(entry);
    }
}
