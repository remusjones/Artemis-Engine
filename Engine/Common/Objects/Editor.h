//
// Created by Remus on 3/02/2024.
//

#pragma once
#include "ImGuiLayer.h"

class Editor : ImGuiLayer{
public:
    void OnImGuiRender() override;
    void DrawProjectFolder();
};
