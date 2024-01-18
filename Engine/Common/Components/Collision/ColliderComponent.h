//
// Created by Remus on 13/01/2024.
//

#pragma once
#include "Components/Component.h"
#include "glm/mat4x4.hpp"
#include "LinearMath/btScalar.h"

class btRigidBody;
class PhysicsSystem;
class btCollisionShape;

struct ColliderCreateInfo {
    btCollisionShape* collisionShape{};
    float linearSleepThreshold = 0.1f;
    float angularSleepThreshold = 0.1f;
    float friction = 0.5f;
    float rollingFriction = 0.05f;
    float spinningFriction = 0.1f;
    btScalar mass{};
};

class ColliderComponent : public Component{
public:
    virtual void Create(PhysicsSystem *aPhysicsSystem, ColliderCreateInfo &aCreateInfo);
    void Tick(float aDeltaTime) override;
    void Initialize() override;

    btRigidBody* GetRigidBody() const { return mRigidBody; }

protected:
    btCollisionShape* mCollisionShape;
    btRigidBody* mRigidBody;
    PhysicsSystem* mPhysicsSystem;

    // Cached Frame to detect external input
    glm::mat4 mWorldMatrixLastFrame;

};
