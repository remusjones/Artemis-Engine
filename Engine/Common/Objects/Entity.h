//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "ImGuiLayer.h"
#include "Math/Transform.h"
class Component;
class Entity : public ImGuiLayer{
public:
    virtual ~Entity() = default;
    virtual void Construct();
    virtual void Tick(float aDeltaTime);
    virtual void Cleanup();
    virtual void AddComponent(Component* aComponent);

    template<class T>
    bool GetComponent(T &aResult);

    template<class T>
    bool GetComponent(std::string name, T &aResult);

    void OnImGuiRender() override;

    Transform mTransform;
    const char *mName{"Default"};

    std::unordered_map<std::string, Component*> mComponentMap;
};

