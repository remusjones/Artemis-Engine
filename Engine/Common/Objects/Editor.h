//
// Created by Remus on 3/02/2024.
//

#pragma once
#include <filesystem>
#include <future>

#include "ImGuiLayer.h"

struct FileInfo {
    std::string fileName;
    std::filesystem::file_time_type lastWriteTime;
};

enum class FileStatus {
    CREATED,
    MODIFIED,
    ERASED
};

class Editor : ImGuiLayer{
public:
    void OnImGuiRender() override;

    static void CreateDirectorySnapshot(const std::string &aPath, std::unordered_map<std::string, FileInfo> &aDirectoryContent);
private:
    void ValidateDirectorySnapshot(const std::unordered_map<std::string, FileInfo> &aDirectoryContent);
    // Draws Directory Content
    void DrawContent();

    void ImportFile(const std::string &aPath, FileStatus status);

    // Create map of directory
    std::unordered_map<std::string, FileInfo> mDirectoryContent;

    std::atomic<bool> mIsFinishedImporting = false;
    std::thread mImportingThread;
    std::mutex mImportingThreadMtx;
};
