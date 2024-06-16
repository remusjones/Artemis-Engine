//
// Created by Remus on 13/01/2024.
//

#include "PhysicsSystem.h"

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "LinearMath/btVector3.h"

void PhysicsSystem::Create() {
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_broadphaseInterface = new btDbvtBroadphase();
    auto *sol = new btSequentialImpulseConstraintSolver();
    m_constraintSolver = sol;

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphaseInterface, m_constraintSolver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0, mGravity, 0));
}

void PhysicsSystem::AwakeRigidBodies()
{
    for (int i = 0; i < m_AllocatedRigidBodies.size(); i++) {
        m_AllocatedRigidBodies[i]->activate(true);
    }
}

void PhysicsSystem::Tick(const float aDeltaTime) {
    if (m_dynamicsWorld) {
        m_dynamicsWorld->stepSimulation(aDeltaTime, 10, 1.f / 240.f);
    }
}

void PhysicsSystem::Destroy() {

    for (int i = 0; i < m_AllocatedRigidBodies.size(); i++) {
        if (m_dynamicsWorld)
            m_dynamicsWorld->removeRigidBody(m_AllocatedRigidBodies[i]);
        delete m_AllocatedRigidBodies[i];
    }

    if (m_dynamicsWorld) {
        int i;

        for (i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--) {
            m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
        }

        for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject *obj = m_dynamicsWorld->getCollisionObjectArray()[i];
            if (btRigidBody *body = btRigidBody::upcast(obj); body && body->getMotionState()) {
                delete body->getMotionState();
            }
            m_dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
    }
    for (int j = 0; j < m_CollisionShapes.size(); j++) {
        delete m_CollisionShapes[j];
    }


    m_CollisionShapes.clear();

    delete m_dynamicsWorld;
    m_dynamicsWorld = nullptr;

    delete m_constraintSolver;
    m_constraintSolver = nullptr;

    delete m_broadphaseInterface;
    m_broadphaseInterface = nullptr;

    delete m_collisionDispatcher;
    m_collisionDispatcher = nullptr;

    delete m_collisionConfiguration;
    m_collisionConfiguration = nullptr;
}

btAlignedObjectArray<btRigidBody *> PhysicsSystem::GetRigidBodies() const {
    return m_AllocatedRigidBodies;
}

btAlignedObjectArray<btCollisionShape *>PhysicsSystem::GetCollisionShapes() const {
    return m_CollisionShapes;
}

btBroadphaseInterface *PhysicsSystem::GetBroadphase() const {
    return m_broadphaseInterface;
}

btCollisionDispatcher *PhysicsSystem::GetDispatcher() const {
    return m_collisionDispatcher;
}

btConstraintSolver *PhysicsSystem::GetConstraintSolver() const {
    return m_constraintSolver;
}

btDefaultCollisionConfiguration *PhysicsSystem::GetCollisionConfiguration() const {
    return m_collisionConfiguration;
}

btDiscreteDynamicsWorld *PhysicsSystem::GetDynamicsWorld() const {
    return m_dynamicsWorld;
}

void PhysicsSystem::AddRigidBody(btRigidBody* aRigidBody) {
    m_dynamicsWorld->addRigidBody(aRigidBody);
    m_AllocatedRigidBodies.push_back(aRigidBody);
    m_CollisionShapes.push_back(aRigidBody->getCollisionShape());
}

void PhysicsSystem::AddCollisionShape(btCollisionShape* aCollisionShape) {
    m_CollisionShapes.push_back(aCollisionShape);
}
