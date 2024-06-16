//
// Created by Remus on 13/01/2024.
//


#include "Component.h"
#include "Objects/Entity.h"

void Component::SetEntity(Entity *aEntity) {
    m_attachedEntity = aEntity;
}

void Component::ClearEntity() {
    m_attachedEntity = nullptr;
}

void Component::SetName(const std::string& aName) {
    mName = aName;
}

std::string Component::GetName() {
    return mName;
}

void Component::Tick(float aDeltaTime) {
}

void Component::Destroy() {
    m_attachedEntity->RemoveComponent(this);
}

void Component::Initialize() {
}
