//
// Created by Remus on 13/01/2024.
//


#include "Component.h"
#include "Objects/Entity.h"

void Component::SetEntity(Entity *aEntity) {
    mAttachedEntity = aEntity;
}

void Component::ClearEntity() {
    mAttachedEntity = nullptr;
}

void Component::SetName(const std::string& aName) {
    mName = aName;
}

std::string Component::GetName() {
    return mName;
}

void Component::Destroy() {
    mAttachedEntity->RemoveComponent(this);
}
