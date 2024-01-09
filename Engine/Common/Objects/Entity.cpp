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

void Entity::OnImGuiRender() {
    mTransform.OnImGuiRender();
}
