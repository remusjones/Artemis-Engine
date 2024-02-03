//
// Created by Remus on 3/02/2024.
//

#include "Editor.h"

#include "VulkanGraphicsImpl.h"
#include "Scenes/Scene.h"

void Editor::OnImGuiRender() {
    DrawProjectFolder();
    gGraphics->mActiveScene->OnImGuiRender();
}

void Editor::DrawProjectFolder() {
}
