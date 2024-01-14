#pragma once
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
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

    void AddCollisionShape(btCollisionShape *const aCollisionShape);

    void RemoveCollisionShape(btCollisionShape *aCollisionShape);

    void Tick(float aDeltaTime);

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
