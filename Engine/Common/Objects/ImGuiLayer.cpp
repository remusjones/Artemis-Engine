//
// Created by Remus on 24/12/2023.
//

#include "ImGuiLayer.h"

#include <random>

#include "glog/logging.h"


ImGuiLayer::ImGuiLayer() {
    mGUID = GetGUID();
}

const char *ImGuiLayer::GetUniqueLabel(const char *aFieldName) const {
    return std::string(std::string(aFieldName) + "##" + mGUID).c_str();
}


static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string ImGuiLayer::GetGUID() {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}
