//
// Created by Remus on 13/01/2024.
//

#pragma once
#include "Components/Component.h"
#include "LinearMath/btScalar.h"

class btRigidBody;
class PhysicsSystem;
class btCollisionShape;

struct ColliderCreateInfo {
    btCollisionShape* collisionShape;
    btScalar mass;
};

class ColliderComponent : public Component{
public:
    virtual void Create(PhysicsSystem *aPhysicsSystem, ColliderCreateInfo &aCreateInfo);
    void Tick(float aDeltaTime) override;
    void Initialize() override;
protected:
    btCollisionShape* mCollisionShape;
    btRigidBody* mRigidBody;
    PhysicsSystem* mPhysicsSystem;

};
