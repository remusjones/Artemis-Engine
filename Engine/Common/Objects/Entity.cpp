//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"

#include <vector>

#include "../Components/Component.h"

void Entity::Construct() {
    for (const auto &component: mComponentMap) {
        component.second->Initialize();
    }
}

void Entity::Tick(const float aDeltaTime) {
    for (const auto &component: mComponentMap) {
        component.second->Tick(aDeltaTime);
    }
}

void Entity::Cleanup() {

    const std::vector<std::pair<std::string, Component*>> copy(mComponentMap.begin(), mComponentMap.end());
    for (const auto &component: copy) {
        component.second->Destroy();
        delete component.second;
    }
}

void Entity::AddComponent(Component *aComponent) {
    aComponent->SetEntity(this);
    mComponentMap[aComponent->GetName()] = aComponent;
}

void Entity::RemoveComponent(Component *aComponent) {
    if (const auto it = mComponentMap.find(aComponent->GetName()); it != mComponentMap.end()) {
        mComponentMap.erase(it);
    }
}

template<typename T>
bool Entity::GetComponent(T& aResult) {
    for (auto &component: mComponentMap) {
        if (dynamic_cast<T *>(component.second)) {
            aResult = static_cast<T *>(component.second);
            return true;
        }
    }
    return false;
}

template<class T>
bool Entity::GetComponent(std::string name, T &aResult) {
    if (mComponentMap.find(name) != mComponentMap.end()) {
        aResult = mComponentMap[name];
        return true;
    }
    return false;
}

void Entity::OnImGuiRender() {
    mTransform.OnImGuiRender();
}
