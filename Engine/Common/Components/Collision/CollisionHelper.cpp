//
// Created by Remus on 15/01/2024.
//

#include "CollisionHelper.h"

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMesh.h"


btTransform CollisionHelper::TransformToBulletTransform(const Transform &aOther) {
    return btTransform(GlmToBullet(aOther.GetLocalRotation()), GlmToBullet(aOther.GetLocalPosition()));
}


btVector3 CollisionHelper::GlmToBullet(const glm::vec3 &aOther) {
    return btVector3(aOther.x, aOther.y, aOther.z);
}

btQuaternion CollisionHelper::GlmToBullet(const glm::quat &aOther) {
    return btQuaternion(aOther.x, aOther.y, aOther.z, aOther.w);
}

glm::vec3 CollisionHelper::BulletToGlm(const btVector3 &aOther) {
    return glm::vec3(aOther.getX(), aOther.getY(), aOther.getZ());
}

glm::quat CollisionHelper::BulletToGlm(const btQuaternion &aOther) {
    return glm::quat(aOther.w(),aOther.x(), aOther.y(), aOther.z());
}

btBvhTriangleMeshShape * CollisionHelper::MakeCollisionMesh(const std::vector<Vertex>& aVertices,
    const std::vector<int32_t>& aIndices) {
    btTriangleMesh* triangle_mesh = new btTriangleMesh(); // Bullet Physics triangle mesh

    for(int i = 0; i<aIndices.size(); i+=3)
    {
        btVector3 vertex0(aVertices[aIndices[i]].mPosition.x, aVertices[aIndices[i]].mPosition.x, aVertices[aIndices[i]].mPosition.z);
        btVector3 vertex1(aVertices[aIndices[i+1]].mPosition.x, aVertices[aIndices[i+1]].mPosition.y, aVertices[aIndices[i+1]].mPosition.z);
        btVector3 vertex2(aVertices[aIndices[i+2]].mPosition.x, aVertices[aIndices[i+2]].mPosition.y, aVertices[aIndices[i+2]].mPosition.z);
        triangle_mesh->addTriangle(vertex0, vertex1, vertex2);
    }

    // Use the btBvhTriangleMeshShape for static triangle mesh
    return new btBvhTriangleMeshShape(triangle_mesh, true);
}

btBoxShape* CollisionHelper::MakeAABBCollision(const std::vector<Vertex>& aVertices)
{
    // Find min and max vertices along each axis
    btVector3 minVertex(aVertices[0].mPosition.x, aVertices[0].mPosition.y, aVertices[0].mPosition.z);
    btVector3 maxVertex(aVertices[0].mPosition.x, aVertices[0].mPosition.y, aVertices[0].mPosition.z);

    for (const Vertex& vertex : aVertices)
    {
        minVertex.setX(std::min(minVertex.x(), vertex.mPosition.x));
        minVertex.setY(std::min(minVertex.y(), vertex.mPosition.y));
        minVertex.setZ(std::min(minVertex.z(), vertex.mPosition.z));

        maxVertex.setX(std::max(maxVertex.x(), vertex.mPosition.x));
        maxVertex.setY(std::max(maxVertex.y(), vertex.mPosition.y));
        maxVertex.setZ(std::max(maxVertex.z(), vertex.mPosition.z));
    }

    // Calculate extents and half extents for the box shape
    const btVector3 extents = maxVertex - minVertex;
    const btVector3 halfExtents = extents * 0.5f;

    // Create and return AABB (Axis-Aligned Bounding Box)
    return new btBoxShape(halfExtents);
}