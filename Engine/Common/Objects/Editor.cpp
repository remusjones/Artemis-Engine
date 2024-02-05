//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"

#include <imgui_internal.h>

#include "FileManagement.h"
#include "imgui.h"
#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::Create(){
    mAssetDirectoryMonitor = std::make_unique<DirectoryMonitor>();
    mAssetDirectoryMonitor->CreateDirectoryMonitor(FileManagement::GetAssetPath());
}

void Editor::OnImGuiRender() {
    //DrawContent();
    gGraphics->mActiveScene->OnImGuiRender();
    DrawContent();
}

void Editor::DrawContent() {

    ImGui::Begin("Project Window");

    if (ImGui::Button(GetUniqueLabel("Refresh Content"))) {
        mAssetDirectoryMonitor->UpdateDirectoryMonitor();
    }

    std::unordered_map<std::string, FileInfo> content = mAssetDirectoryMonitor->GetDirectoryContent();
    if (!content.empty() && ImGui::BeginTable("3D Files", content.size() - 1, ImGuiTableFlags_BordersOuterH |
    ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg), ImVec2(110, 110), 110.0f)
    {
        for(auto& [key, value] : content)
        {
            // TODO: incorporate row usage
            ImGui::Button("", ImVec2(100,100)); // Placeholder for thumbnail button
            ImGui::Text("%s", value.fileName.c_str());
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    mAssetDirectoryMonitor->OnImGuiRender();
    ImGui::End();
}
