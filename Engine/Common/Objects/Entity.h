//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "ImGuiLayer.h"
#include "Math/Transform.h"

class Entity : public ImGuiLayer{
public:
    virtual ~Entity() = default;
    virtual void Construct();
    virtual void Tick(float aDeltaTime);
    virtual void Cleanup();
    void OnImGuiRender() override;

    Transform mTransform;
    const char *mName{"Default"};
};
