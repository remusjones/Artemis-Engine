//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Objects/Camera.h"
#include "Objects/Camera.h"

Transform::Transform(): mRotation(glm::identity<glm::quat>()), mPosition(0), mScale(1) {
}

glm::vec3 Transform::Position() const {
    return mPosition;
}

glm::quat Transform::Rotation() const {
    return mRotation;
}

glm::vec3 Transform::Euler() const {
    return glm::degrees(glm::eulerAngles(mRotation));
}

glm::vec3 Transform::Scale() const {
    return mScale;
}

void Transform::Translate(glm::vec3 aDirection) {
    mPosition += aDirection;
}

void Transform::SetPosition(const glm::vec3 aNewPosition) {
    mPosition = aNewPosition;
}

void Transform::SetRotation(glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);
    mRotation = axisRotation;
}

void Transform::SetRotation(glm::quat aNewRotation) {
    mRotation = aNewRotation;
}

void Transform::RotateAxis(float aAngle, glm::vec3 aRotation) {
    mRotation = mRotation * glm::angleAxis(aAngle, aRotation);
}

void Transform::RotateAxis(const glm::vec2 &aEulerAxisRotation) {
    const glm::vec2 axisRotation = glm::radians(aEulerAxisRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;


}

void Transform::RotateAxis(glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;
    mRotation = mRotation * glm::angleAxis(axisRotation.z, glm::vec3(0, 0, 1));
}

void Transform::Rotate(glm::quat aRotation) {
    mRotation *= aRotation;
}


void Transform::SetScale(glm::vec3 aNewScale) {
    mScale = aNewScale;
}

glm::mat4 Transform::GetWorldMatrix() const {
    glm::mat4 worldMatrix = glm::identity<glm::mat4>();

    worldMatrix = glm::translate(glm::mat4(1.0f), mPosition);
    worldMatrix *= glm::mat4_cast(mRotation);
    worldMatrix *= glm::scale(glm::mat4(1.0f), mScale);

    return worldMatrix;
}
