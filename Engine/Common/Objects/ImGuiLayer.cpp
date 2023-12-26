//
// Created by Remus on 24/12/2023.
//

#include "ImGuiLayer.h"

int32_t ImGuiLayer::sInstance = 0;

ImGuiLayer::ImGuiLayer() {
    sInstance++;
    mInstance = sInstance;
}

const char *ImGuiLayer::GetHashedName(const char *aFieldName) const {
    return std::string(std::string(aFieldName) + "##" + std::to_string(mInstance)).c_str();
}
