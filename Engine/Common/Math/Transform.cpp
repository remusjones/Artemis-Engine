//
// Created by Remus on 22/12/2023.
//

#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

#include "imgui.h"
#include "glm/gtx/matrix_decompose.hpp"

Transform::Transform(): mRotation(glm::identity<glm::quat>()), mPosition(0), mScale(1), mLocalMatrix(),
                        mMatrixDirty(true), mParent(nullptr) {
}

glm::vec3 Transform::GetWorldPosition(){
    return glm::vec3(GetWorldMatrix()[3]);
}

glm::quat Transform::GetWorldRotation() {
    glm::mat4 matrix = GetWorldMatrix();
    glm::mat3 upper_mat3(matrix);
    return glm::quat(upper_mat3);
}

glm::vec3 Transform::GetWorldScale() {
    glm::mat4 matrix = GetWorldMatrix();
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(matrix[0]));
    scale.y = glm::length(glm::vec3(matrix[1]));
    scale.z = glm::length(glm::vec3(matrix[2]));
    return scale;
}
glm::vec3 Transform::GetLocalPosition() const {
    return mPosition;
}

glm::quat Transform::GetLocalRotation() const {
    return mRotation;
}

glm::vec3 Transform::GetLocalEuler() const {
    return glm::degrees(glm::eulerAngles(mRotation));
}

glm::vec3 Transform::GetLocalScale() const {
    return mScale;
}

void Transform::Translate(const glm::vec3 aTranslation) {
    mPosition += aTranslation;
    SetDirty();
}

void Transform::TranslateLocal(const glm::vec3 aTranslation) {
    mPosition += aTranslation * mRotation;
    SetDirty();
}

void Transform::SetLocalPosition(const glm::vec3 aNewPosition) {
    mPosition = aNewPosition;
    SetDirty();
}

void Transform::SetLocalRotation(const glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);
    mRotation = axisRotation;
    SetDirty();
}

void Transform::SetLocalRotation(const glm::quat aNewRotation) {
    mRotation = aNewRotation;
    SetDirty();
}

void Transform::SetWorldPosition(const glm::vec3 aNewPosition) {
    if (mParent != nullptr)
    {
        const glm::vec3 parentPosition = mParent->GetWorldPosition();
        mPosition = aNewPosition - parentPosition;
    }
    else
    {
        mPosition = aNewPosition;
    }
    SetDirty();
}

void Transform::SetWorldRotation(const glm::quat aNewRotation)
{
    if (mParent != nullptr)
    {
        const glm::quat parentRotation = mParent->GetWorldRotation();
        mRotation = glm::inverse(parentRotation) * aNewRotation;
    }
    else
    {
        mRotation = aNewRotation;
    }
    SetDirty();
}

void Transform::SetWorldScale(const glm::vec3 aNewScale)
{
    if (mParent != nullptr)
    {
        const glm::vec3 parentScale = mParent->GetWorldScale();
        mScale = aNewScale / parentScale;
    }
    else
    {
        mScale = aNewScale;
    }
    SetDirty();
}

void Transform::RotateAxisLocal(float aAngle, glm::vec3 aRotation) {
    mRotation = mRotation * glm::angleAxis(aAngle, aRotation);
    SetDirty();
}

void Transform::RotateAxisLocal(const glm::vec2 &aEulerAxisRotation) {
    const glm::vec2 axisRotation = glm::radians(aEulerAxisRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;
    SetDirty();
}

void Transform::RotateAxisLocal(glm::vec3 aEulerRotation) {
    const glm::vec3 axisRotation = glm::radians(aEulerRotation);

    mRotation = mRotation * glm::angleAxis(axisRotation.x, glm::vec3(1, 0, 0));
    mRotation = glm::angleAxis(axisRotation.y, glm::vec3(0, 1, 0)) * mRotation;
    mRotation = mRotation * glm::angleAxis(axisRotation.z, glm::vec3(0, 0, 1));
    SetDirty();
}

void Transform::LocalRotate(glm::quat aRotation) {
    mRotation *= aRotation;
    SetDirty();
}


void Transform::SetLocalScale(glm::vec3 aNewScale) {
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

void Transform::SetParent(Transform *aParent) {

    if (mParent != nullptr)
        mParent->RemoveChild(this);

    mParent = aParent;

    if (mParent != nullptr)
    {
        mParent->AddChild(this);
        mParent->SetDirty();
    }


    SetDirty();
}

void Transform::RemoveChild(Transform *aChild) {
    if (const auto it = std::find(mChildren.begin(), mChildren.end(), aChild); it != mChildren.end())
        mChildren.erase(it);
    aChild->SetParent(nullptr);
    mParent->SetDirty();
}

void Transform::AddChild(Transform *aChild) {
    mChildren.push_back(aChild);
    aChild->SetDirty();
}

void Transform::UpdateLocalMatrix() {
    mLocalMatrix = glm::mat4(1.0f);
    mLocalMatrix = glm::translate(mLocalMatrix, mPosition);
    mLocalMatrix = mLocalMatrix * glm::mat4_cast(mRotation);
    mLocalMatrix = glm::scale(mLocalMatrix, mScale);
}

glm::mat4 Transform::GetWorldMatrix() {
    if (mMatrixDirty) {
        UpdateLocalMatrix();
        for (const auto child: mChildren) {
            child->SetDirty();
        }
        mMatrixDirty = false;
    }
    if (mParent)
        return mParent->GetWorldMatrix() * mLocalMatrix;

    return mLocalMatrix;
}

void Transform::OnImGuiRender() {
    ImGui::SeparatorText("Transform");
    glm::vec3 rot = GetLocalEuler();
    if (ImGui::DragFloat3(GetUniqueLabel("Position"), &mPosition[0], 0.1f)) {
        SetDirty();
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Rotation"), &rot[0], 0.1f)) {
        SetLocalRotation(rot);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Scale"), &mScale[0], 0.1f)) {
        SetDirty();
    }
}
