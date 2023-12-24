//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Objects/Camera.h"
#include "Objects/Camera.h"
#include "Objects/Camera.h"
#include "Objects/Camera.h"

Transform::Transform(): mTranslationMatrix(1.0f), mRotationMatrix(1.0f), mScaleMatrix(1.0f) {
}

glm::vec3 Transform::Position() const {
    return glm::vec3(mTranslationMatrix[3]);
}

glm::quat Transform::Rotation() const {
    return glm::quat_cast(mRotationMatrix);
}

glm::vec3 Transform::Euler() const {
    return glm::degrees(glm::eulerAngles(glm::quat_cast(mRotationMatrix)));
}

glm::vec3 Transform::Scale() const {
    glm::vec3 scale;
    scale.x = glm::length(mScaleMatrix[0]);
    scale.y = glm::length(mScaleMatrix[1]);
    scale.z = glm::length(mScaleMatrix[2]);
    return scale;
}

void Transform::SetPosition(const glm::vec3 aNewPosition) {
   mTranslationMatrix = translate(glm::mat4(1.0f), aNewPosition);
}

void Transform::SetRotation(glm::vec3 aNewRotation) {
    mRotationMatrix = glm::eulerAngleYXZ(
        aNewRotation.y / 180 * glm::pi<float>(),
        aNewRotation.x/ 180 * glm::pi<float>(),
        aNewRotation.z/ 180 * glm::pi<float>());
}

void Transform::RotateAround(float aAngle, glm::vec3 aRotation) {
    mRotationMatrix = glm::rotate(mRotationMatrix, aAngle, aRotation);
}

void Transform::SetScale(glm::vec3 aNewScale) {
    mScaleMatrix = glm::scale(glm::mat4(1.0f), aNewScale);
}

glm::mat4 Transform::GetCombinedMatrix() const {
    return mTranslationMatrix * mRotationMatrix * mScaleMatrix;
}
