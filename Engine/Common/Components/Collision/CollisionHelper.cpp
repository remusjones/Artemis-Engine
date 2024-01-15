//
// Created by Remus on 15/01/2024.
//

#include "CollisionHelper.h"

btTransform CollisionHelper::TransformToBulletTransform(const Transform &aOther) {
    return btTransform(GlmToBullet(aOther.Rotation()), GlmToBullet(aOther.Position()));
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
    return glm::quat(aOther.getW(),aOther.getX(), aOther.getY(), aOther.getZ());
}
