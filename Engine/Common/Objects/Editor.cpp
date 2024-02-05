//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"

#include <filesystem>
#include <thread>

#include "imgui.h"
#include "Logger.h"
#include "VulkanGraphicsImpl.h"
#include "File Management/FileManagement.h"
#include "Scenes/Scene.h"

void Editor::OnImGuiRender() {
    //DrawContent();
    gGraphics->mActiveScene->OnImGuiRender();
    DrawContent();
}

void Editor::CreateDirectorySnapshot(const std::string &aPath, std::unordered_map<std::string, FileInfo> &aDirectoryContent) {
    for (const std::filesystem::path path = aPath; const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
        if (!aDirectoryContent.contains(entry.path().string())) {
            aDirectoryContent[entry.path().string()] = FileInfo{
                entry.path().filename().string(),
                last_write_time(entry)
            };
        }
    }
}

void Editor::ValidateDirectorySnapshot(const std::unordered_map<std::string, FileInfo> &aDirectoryContent) {

    // Compare content with existing content, and update if necessary
    for(const auto& entry : aDirectoryContent) {
        if (mDirectoryContent.contains(entry.first)) { // File exists
            if (mDirectoryContent[entry.first].lastWriteTime != entry.second.lastWriteTime) {
                ImportFile(entry.first, FileStatus::MODIFIED);
            }
        }
        else { // File is new
            ImportFile(entry.first, FileStatus::CREATED);
        }
    }
    for(const auto& entry : mDirectoryContent) {
        if (!aDirectoryContent.contains(entry.first)) { // File is erased
            ImportFile(entry.first, FileStatus::ERASED);
        }
    }
    mDirectoryContent = aDirectoryContent;
}

void Editor::DrawContent() {

    ImGui::Begin("Project Window");
    if (ImGui::Button(GetUniqueLabel("Refresh Content"))) {
        std::lock_guard lock(mImportingThreadMtx);
        if (mImportingThread.joinable()) {
            mImportingThread.join();
        }
        mImportingThread = std::thread([this] {
            mIsFinishedImporting = false;
            std::unordered_map<std::string, FileInfo> newDirectoryContent;
            CreateDirectorySnapshot(FileManagement::GetAssetPath(), newDirectoryContent);
            ValidateDirectorySnapshot(newDirectoryContent);
            mIsFinishedImporting = true;
        });
    }
    ImGui::End();

    if (mImportingThread.joinable()) {
        std::lock_guard lock(mImportingThreadMtx);
        if (mIsFinishedImporting) {
            mImportingThread.join();
        }
    }
}
void Editor::ImportFile(const std::string &aPath, FileStatus status) {
    // switch
    switch (status) {
        case FileStatus::CREATED:
            Logger::Log("File CREATED: " + aPath);
            break;
        case FileStatus::MODIFIED:
            Logger::Log("File MODIFIED: " + aPath);
            break;
        case FileStatus::ERASED:
            Logger::Log("File ERASED: " + aPath);
            break;
        default: ;
    }

}
