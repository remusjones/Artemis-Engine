//
// Created by Remus on 15/01/2024.
//

#pragma once
#include "Base/Common/Data/Vertex.h"
#include "LinearMath/btTransform.h"
#include "Math/Transform.h"


class btBoxShape;
class btBvhTriangleMeshShape;

class CollisionHelper {
public:
    static btTransform TransformToBulletTransform(const Transform& aOther);
    static btVector3 GlmToBullet(const glm::vec3& aOther);
    static btQuaternion GlmToBullet(const glm::quat& aOther);
    static glm::vec3 BulletToGlm(const btVector3& aOther);
    static glm::quat BulletToGlm(const btQuaternion& aOther);
    static btBvhTriangleMeshShape* MakeCollisionMesh(const std::vector<Vertex>& aVertices, const std::vector<int32_t>&
    aIndices);
    static btBoxShape* MakeAABBCollision(const std::vector<Vertex>& aVertices);
};
