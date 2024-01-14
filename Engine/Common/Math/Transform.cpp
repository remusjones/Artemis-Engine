//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "imgui.h"

Transform::Transform(): mRotation(glm::identity<glm::quat>()), mPosition(0), mScale(1), mWorldMatrix(1),
                        mMatrixDirty(true) {
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

void Transform::Translate(glm::vec3 aTranslation) {
    mPosition += aTranslation;
    SetDirty();
}

void Transform::TranslateLocal(glm::vec3 aTranslation) {
    mPosition += aTranslation * mRotation;
    SetDirty();
}

void Transform::SetPosition(const glm::vec3 aNewPosition) {
    mPosition = aNewPosition;
    SetDirty();
}

void Transform::SetRotation(glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);
    mRotation = axisRotation;
    SetDirty();
}

void Transform::SetRotation(glm::quat aNewRotation) {
    mRotation = aNewRotation;
    SetDirty();
}

void Transform::RotateAxis(float aAngle, glm::vec3 aRotation) {
    mRotation = mRotation * glm::angleAxis(aAngle, aRotation);
    SetDirty();
}

void Transform::RotateAxis(const glm::vec2 &aEulerAxisRotation) {
    const glm::vec2 axisRotation = glm::radians(aEulerAxisRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;
    SetDirty();
}

void Transform::RotateAxis(glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;
    mRotation = mRotation * glm::angleAxis(axisRotation.z, glm::vec3(0, 0, 1));
    SetDirty();
}

void Transform::Rotate(glm::quat aRotation) {
    mRotation *= aRotation;
    SetDirty();
}


void Transform::SetScale(glm::vec3 aNewScale) {
    mScale = aNewScale;
    SetDirty();
}

void Transform::SetMatrix(glm::mat4 aMatrix) {
    mPosition = glm::vec3(aMatrix[3][0], aMatrix[3][1], aMatrix[3][2]);

    mScale.x = glm::length(glm::vec3(aMatrix[0][0], aMatrix[0][1], aMatrix[0][2]));
    mScale.y = glm::length(glm::vec3(aMatrix[1][0], aMatrix[1][1], aMatrix[1][2]));
    mScale.z = glm::length(glm::vec3(aMatrix[2][0], aMatrix[2][1], aMatrix[2][2]));

    glm::mat3 rotation;
    rotation[0] = glm::normalize(glm::vec3(aMatrix[0]));
    rotation[1] = glm::normalize(glm::vec3(aMatrix[1]));
    rotation[2] = glm::normalize(glm::vec3(aMatrix[2]));
    mRotation = glm::quat(rotation);

    SetDirty();
}

void Transform::SetDirty() {
    mMatrixDirty = true;
}

bool Transform::GetDirty() const {
    return mMatrixDirty;
}

glm::mat4 Transform::GetWorldMatrix() {

    if (mMatrixDirty) {
        mWorldMatrix = glm::translate(glm::mat4(1.0f), mPosition);
        mWorldMatrix *= glm::mat4_cast(mRotation);
        mWorldMatrix *= glm::scale(glm::mat4(1.0f), mScale);
        mMatrixDirty = false;
    }

    return mWorldMatrix;
}

void Transform::OnImGuiRender() {
    ImGui::SeparatorText("Transform");
    glm::vec3 rot = Euler();
    if (ImGui::DragFloat3(GetUniqueLabel("Position"), &mPosition[0], 0.1f)) {
        SetDirty();
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Rotation"), &rot[0], 0.1f)) {
        SetRotation(rot);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Scale"), &mScale[0], 0.1f)) {
        SetDirty();
    }
}
