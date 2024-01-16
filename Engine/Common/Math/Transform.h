//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <algorithm>
#include <vector>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Objects/ImGuiLayer.h"


class Transform : public ImGuiLayer {
public:
    Transform();

    //TODO: Make vectors constant
    glm::vec3 Forward() const { return mRotation * glm::vec3(0, 0, 1); }
    glm::vec3 Up() const { return mRotation * glm::vec3(0, 1, 0); }
    glm::vec3 Right() const { return mRotation * glm::vec3(1, 0, 0); }

    glm::vec3 GetLocalPosition() const;

    glm::quat GetLocalRotation() const;

    glm::vec3 GetLocalEuler() const;

    glm::vec3 GetLocalScale() const;

    glm::vec3 GetWorldPosition();

    glm::quat GetWorldRotation();

    glm::vec3 GetWorldScale();

    void Translate(glm::vec3 aTranslation);

    void TranslateLocal(glm::vec3 aTranslation);

    void SetLocalPosition(glm::vec3 aNewPosition);

    void SetLocalRotation(glm::vec3 aEulerRotation);

    void SetLocalRotation(glm::quat aNewRotation);

    void SetWorldPosition(glm::vec3 aNewPosition);

    void SetWorldRotation(glm::quat aNewRotation);

    void RotateAxisLocal(float aAngle, glm::vec3 aRotation);

    void RotateAxisLocal(const glm::vec2 &aEulerAxisRotation);

    void RotateAxisLocal(glm::vec3& aEulerRotation);

    void RotateAxisWorld(float aAngle, glm::vec3 aRotation);

    void RotateAxisWorld(const glm::vec2 &aEulerAxisRotation);

    void RotateAxisWorld(glm::vec3 aEulerRotation);


    void LocalRotate(glm::quat aRotation);

    void SetLocalScale(glm::vec3 aNewScale);

    void SetWorldScale(glm::vec3 aNewScale);

    void SetMatrix(glm::mat4 aMatrix);

    void SetDirty();

    bool GetDirty() const;

    void SetParent(Transform *aParent);

    glm::mat4 GetWorldMatrix();

    glm::mat4 GetRotationMatrix() const { return glm::mat4_cast(mRotation); }
    glm::mat4 GetTranslationMatrix() const { return glm::translate(glm::identity<glm::mat4>(), mPosition); }
    glm::mat4 GetScaleMatrix() const { return glm::scale(glm::identity<glm::mat4>(), mScale); }

    void OnImGuiRender() override;

private:
    glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;

    glm::mat4 mLocalMatrix;
    bool mMatrixDirty;

    Transform *mParent;
    std::vector<Transform *> mChildren;

    void RemoveChild(Transform *aChild);

    void AddChild(Transform *aChild);

    void UpdateLocalMatrix();
};
