//
// Created by Remus on 13/01/2024.
//

#include "ColliderComponent.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorldImporter.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "glm/gtc/type_ptr.hpp"
#include "Objects/Entity.h"
#include "Physics/PhysicsSystem.h"

void ColliderComponent::Create(PhysicsSystem *aPhysicsSystem, ColliderCreateInfo &aCreateInfo) {
    mPhysicsSystem = aPhysicsSystem;
    btVector3 localInertia;

    if (aCreateInfo.mass > 0)
        aCreateInfo.collisionShape->calculateLocalInertia(aCreateInfo.mass, localInertia);

    mRigidBody = new btRigidBody(aCreateInfo.mass, nullptr, aCreateInfo.collisionShape, localInertia);
    aPhysicsSystem->mDynamicsWorld->addRigidBody(mRigidBody);
    aPhysicsSystem->mAllocatedRigidBodies.push_back(mRigidBody);
}

void ColliderComponent::Tick(const float aDeltaTime) {
    Component::Tick(aDeltaTime);
    if (mRigidBody) {

        // Convert rigidbody calc into transform
        glm::mat4 rigidbodyMatrix;
        mRigidBody->getWorldTransform().getOpenGLMatrix(glm::value_ptr(rigidbodyMatrix));
        mAttachedEntity->mTransform.SetMatrix(rigidbodyMatrix * mAttachedEntity->mTransform.GetWorldMatrix());
    }
}

void ColliderComponent::Initialize() {
    Component::Initialize();
    // TODO: Initialize Correctly 
  //  mRigidBody->setWorldTransform(btTransform(glm::value_ptr(mAttachedEntity->mTransform.GetWorldMatrix())))
    //if (mRigidBody) {
    //    mRigidBody->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(mAttachedEntity->mTransform
    //.GetWorldMatrix()));
    //}
}
