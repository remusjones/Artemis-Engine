//
// Created by Remus on 13/01/2024.
//

#include "PhysicsSystem.h"

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "LinearMath/btVector3.h"

void PhysicsSystem::Create() {
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mBroadphase = new btDbvtBroadphase();
    auto *sol = new btSequentialImpulseConstraintSolver;
    mSolver = sol;
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
    mDynamicsWorld->setGravity(btVector3(0, mGravity, 0));
}

void PhysicsSystem::Tick(const float aDeltaTime) {
    if (mDynamicsWorld) {
        mDynamicsWorld->stepSimulation(aDeltaTime);
    }
}

void PhysicsSystem::Destroy() {
    if (mDynamicsWorld) {
        int i;
        for (i = mDynamicsWorld->getNumConstraints() - 1; i >= 0; i--) {
            mDynamicsWorld->removeConstraint(mDynamicsWorld->getConstraint(i));
        }

        for (i = mDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject *obj = mDynamicsWorld->getCollisionObjectArray()[i];
            if (btRigidBody *body = btRigidBody::upcast(obj); body && body->getMotionState()) {
                delete body->getMotionState();
            }
            mDynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
    }
    for (int j = 0; j < mCollisionShapes.size(); j++) {
        delete mCollisionShapes[j];
    }

    for (int k = 0; k < mAllocatedRigidBodies.size(); k++) {
        if (mDynamicsWorld)
            mDynamicsWorld->removeRigidBody(mAllocatedRigidBodies[k]);
        delete mAllocatedRigidBodies[k];
    }
    mCollisionShapes.clear();

    delete mDynamicsWorld;
    mDynamicsWorld = nullptr;

    delete mSolver;
    mSolver = nullptr;

    delete mBroadphase;
    mBroadphase = nullptr;

    delete mDispatcher;
    mDispatcher = nullptr;

    delete mCollisionConfiguration;
    mCollisionConfiguration = nullptr;
}
