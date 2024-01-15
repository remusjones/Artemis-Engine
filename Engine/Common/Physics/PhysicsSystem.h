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

    void Tick(float aDeltaTime) const;

    void Destroy();

    btAlignedObjectArray<btCollisionShape *> mCollisionShapes;
    btAlignedObjectArray<btRigidBody*> mAllocatedRigidBodies;

    btBroadphaseInterface *mBroadphase;
    btCollisionDispatcher *mDispatcher;
    btConstraintSolver *mSolver;
    btDefaultCollisionConfiguration *mCollisionConfiguration;
    btDiscreteDynamicsWorld *mDynamicsWorld;

    float_t mGravity = -9.81f;
};