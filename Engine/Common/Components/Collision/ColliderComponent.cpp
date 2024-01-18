//
// Created by Remus on 13/01/2024.
//

#include "ColliderComponent.h"

#include "CollisionHelper.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorldImporter.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "glm/gtc/type_ptr.hpp"
#include "LinearMath/btDefaultMotionState.h"
#include "Objects/Entity.h"
#include "Physics/PhysicsSystem.h"

void ColliderComponent::Create(PhysicsSystem *aPhysicsSystem, ColliderCreateInfo &aCreateInfo) {
    mPhysicsSystem = aPhysicsSystem;
    btVector3 localInertia;

    if (aCreateInfo.mass > 0)
        aCreateInfo.collisionShape->calculateLocalInertia(aCreateInfo.mass, localInertia);

    auto *myMotionState = new btDefaultMotionState();
    const btRigidBody::btRigidBodyConstructionInfo cInfo(aCreateInfo.mass, myMotionState, aCreateInfo.collisionShape,
                                                         localInertia);
    mRigidBody = new btRigidBody(cInfo);
    mRigidBody->setUserIndex(-1);
    mRigidBody->setSleepingThresholds(aCreateInfo.linearSleepThreshold, aCreateInfo.angularSleepThreshold);
    mRigidBody->setFriction(aCreateInfo.friction);
    mRigidBody->setRollingFriction(aCreateInfo.rollingFriction);
    mRigidBody->setSpinningFriction(aCreateInfo.spinningFriction);

    aPhysicsSystem->mDynamicsWorld->addRigidBody(mRigidBody);
    aPhysicsSystem->mAllocatedRigidBodies.push_back(mRigidBody);
    mCollisionShape = aCreateInfo.collisionShape;
    aPhysicsSystem->mCollisionShapes.push_back(aCreateInfo.collisionShape); // TODO: Do collision shape instancing ..
}

void ColliderComponent::Tick(const float aDeltaTime) {
    Component::Tick(aDeltaTime);

    if (mRigidBody) {

        // Test to see whether matrix was modified externally since last frame
        // TODO: investigate on how to only wake up nearby colliders
        if (mWorldMatrixLastFrame != mAttachedEntity->mTransform.GetWorldMatrix()) {
            mRigidBody->proceedToTransform(CollisionHelper::TransformToBulletTransform(mAttachedEntity->mTransform));
            mPhysicsSystem->AwakeRigidBodies();
        }

        glm::mat4 rigidBodyMatrix;
        mRigidBody->getWorldTransform().getOpenGLMatrix(value_ptr(rigidBodyMatrix));

        mAttachedEntity->mTransform.SetWorldPosition(CollisionHelper::BulletToGlm(
            mRigidBody->getWorldTransform().getOrigin()));

        mAttachedEntity->mTransform.SetWorldRotation(CollisionHelper::BulletToGlm(
            mRigidBody->getWorldTransform().getRotation()));

        mWorldMatrixLastFrame = mAttachedEntity->mTransform.GetWorldMatrix();
    }
}

void ColliderComponent::Initialize() {
    Component::Initialize();
    mRigidBody->setWorldTransform(CollisionHelper::TransformToBulletTransform(mAttachedEntity->mTransform));
    mWorldMatrixLastFrame = mAttachedEntity->mTransform.GetWorldMatrix();
}
