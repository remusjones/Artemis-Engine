//
// Created by Remus on 13/01/2024.
//

#include "ColliderComponent.h"

void ColliderComponent::Create(PhysicsSystem* aPhysicsSystem) {
    mPhysicsSystem = aPhysicsSystem;
    // Add collider and generate physics here
}

void ColliderComponent::Tick(float aDeltaTime) {
    Component::Tick(aDeltaTime);
}

void ColliderComponent::Initialize() {
    Component::Initialize();

}
