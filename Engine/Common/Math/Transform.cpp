//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

Transform::Transform() {
    mTransformationMatrix = glm::mat4(1.f);
}

glm::vec3 Transform::Position() const {
    return glm::vec3(mTransformationMatrix[3]);
}

glm::quat Transform::Rotation() const {
    return glm::quat_cast(mTransformationMatrix);
}

glm::vec3 Transform::Euler() const {
    return glm::eulerAngles(glm::quat_cast(mTransformationMatrix));
}

glm::vec3 Transform::Scale() const {
    glm::vec3 scale;
    scale.x = glm::length(mTransformationMatrix[0]);
    scale.y = glm::length(mTransformationMatrix[1]);
    scale.z = glm::length(mTransformationMatrix[2]);
    return scale;
}

void Transform::SetPosition(const glm::vec3 aNewPosition) {
    mTransformationMatrix = translate(mTransformationMatrix, aNewPosition * Scale());
}

void Transform::SetRotation(glm::vec3 aNewRotation) {
    const glm::mat4 transform1 = glm::eulerAngleYXZ(aNewRotation.y, aNewRotation.x, aNewRotation.z);
    mTransformationMatrix *= transform1;
}

void Transform::SetScale(glm::vec3 aNewScale) {
    mTransformationMatrix *= glm::scale(glm::mat4(1.f), aNewScale);
}
