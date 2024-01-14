//
// Created by Remus on 13/01/2024.
//

#include "ColliderComponent.h"

#include "Logger.h"
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
    aPhysicsSystem->mDynamicsWorld->addRigidBody(mRigidBody);
    aPhysicsSystem->mAllocatedRigidBodies.push_back(mRigidBody);
    aPhysicsSystem->mCollisionShapes.push_back(aCreateInfo.collisionShape); // TODO: Do collision shape instancing ..
}

void ColliderComponent::Tick(const float aDeltaTime) {
    Component::Tick(aDeltaTime);

    if (mRigidBody) {
        if (mWorldMatrixLastFrame != mAttachedEntity->mTransform.GetWorldMatrix()) {
            mRigidBody->getWorldTransform().setFromOpenGLMatrix(
                glm::value_ptr(mAttachedEntity->mTransform.GetWorldMatrix()));
            mRigidBody->activate(true);
        }

        glm::mat4 rigidBodyMatrix;
        mRigidBody->getWorldTransform().getOpenGLMatrix(glm::value_ptr(rigidBodyMatrix));
        mAttachedEntity->mTransform.SetMatrix(rigidBodyMatrix);
    }
    mWorldMatrixLastFrame = mAttachedEntity->mTransform.GetWorldMatrix();
}

void ColliderComponent::Initialize() {
    Component::Initialize();
    mRigidBody->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(mAttachedEntity->mTransform.GetWorldMatrix()));
}
