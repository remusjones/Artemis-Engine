//
// Created by Remus on 5/02/2024.
//

#pragma once
#include "Objects/ImGuiLayer.h"


class ContentItem : ImGuiLayer {
public:
    void Create();
    void OnImGuiRender() override;
};
