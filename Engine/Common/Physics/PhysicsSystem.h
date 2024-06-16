#pragma once
#include "LinearMath/btAlignedObjectArray.h"


class btRigidBody;
class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btConstraintSolver;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btCollisionShape;

class PhysicsSystem {
public:

    void Create();
    void AwakeRigidBodies();
    void Tick(float aDeltaTime);
    void Destroy();
    void AddRigidBody(btRigidBody* aRigidBody);
    void AddCollisionShape(btCollisionShape* aCollisionShape);

    [[nodiscard]] btAlignedObjectArray <btRigidBody*> GetRigidBodies() const;
    [[nodiscard]] btAlignedObjectArray <btCollisionShape*> GetCollisionShapes() const;
    [[nodiscard]] btBroadphaseInterface* GetBroadphase() const;
    [[nodiscard]] btCollisionDispatcher* GetDispatcher() const;
    [[nodiscard]] btConstraintSolver* GetConstraintSolver() const;
    [[nodiscard]] btDefaultCollisionConfiguration* GetCollisionConfiguration() const;
    [[nodiscard]] btDiscreteDynamicsWorld* GetDynamicsWorld() const;


private:
    btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
    btAlignedObjectArray<btRigidBody*> m_AllocatedRigidBodies;

    btBroadphaseInterface* m_broadphaseInterface;
    btCollisionDispatcher* m_collisionDispatcher;
    btConstraintSolver* m_constraintSolver;
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btDiscreteDynamicsWorld* m_dynamicsWorld;

    float_t mGravity = -9.81f;
};