//
// Created by Remus on 24/12/2023.
//

#pragma once
#include <cstdint>
#include <string>


class ImGuiLayer {
public:
    ImGuiLayer();

    virtual void OnImGuiRender() = 0; // TODO: Invoke from single point in app

    const char* GetHashedName(const char* aFieldName) const;

private:
    int32_t mInstance;
    static int32_t sInstance;
};
