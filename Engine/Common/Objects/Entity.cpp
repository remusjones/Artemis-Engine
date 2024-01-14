//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"

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
    for (const auto &component: mComponentMap) {
        component.second->Destroy();
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
