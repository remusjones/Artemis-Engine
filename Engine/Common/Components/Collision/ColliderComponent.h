//
// Created by Remus on 13/01/2024.
//

#pragma once
#include "Components/Component.h"

class PhysicsSystem;
class btCollisionShape;

class ColliderComponent : public Component{
public:
    virtual void Create(PhysicsSystem* aPhysicsSystem);
    void Tick(float aDeltaTime) override;
    void Initialize() override;
protected:
    btCollisionShape* mCollisionShape;
    PhysicsSystem* mPhysicsSystem;

};
