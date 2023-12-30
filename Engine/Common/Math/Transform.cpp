//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Objects/Camera.h"
#include "Objects/Camera.h"

Transform::Transform(): mRotation(glm::identity<glm::quat>()), mPosition(0), mScale(1){
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

void Transform::Rotate(glm::quat aRotation) {
    mRotation *= aRotation;
}

void Transform::RotateAround(float aAngle, glm::vec3 aRotation) {
    mRotation = mRotation * glm::angleAxis(aAngle, aRotation);
    //mRotationMatrix = glm::rotate(mRotationMatrix, aAngle, aRotation);
}

void Transform::SetScale(glm::vec3 aNewScale) {
    mScale = aNewScale;
}

glm::mat4 Transform::GetCombinedMatrix() const {

    glm::mat4 worldMatrix = glm::identity<glm::mat4>();

    worldMatrix = glm::translate(glm::identity<glm::mat4>(), mPosition);
    worldMatrix *= glm::mat4_cast(mRotation);
    worldMatrix = glm::scale(worldMatrix, mScale);

    return worldMatrix;
}
