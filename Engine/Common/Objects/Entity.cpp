//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"

void Entity::Construct() {
}

void Entity::Tick(float aDeltaTime) {
}

void Entity::Cleanup() {
}

void Entity::AddComponent(Component *aComponent) {
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
