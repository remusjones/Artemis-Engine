//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Objects/ImGuiLayer.h"


class Transform : public ImGuiLayer{
public:
    Transform();

    //TODO: Make vectors constant
    glm::vec3 Forward() const { return mRotation * glm::vec3(0, 0, 1); }
    glm::vec3 Up() const { return mRotation * glm::vec3(0, 1, 0); }
    glm::vec3 Right() const { return mRotation * glm::vec3(1, 0, 0); }

    glm::vec3 Position() const;
    glm::quat Rotation() const;
    glm::vec3 Euler() const;
    glm::vec3 Scale() const;

    void Translate(glm::vec3 aTranslation);
    void TranslateLocal(glm::vec3 aTranslation);
    void SetPosition(glm::vec3 aNewPosition);

    void SetRotation(glm::vec3 aEulerRotation);
    void SetRotation(glm::quat aNewRotation);

    void RotateAxis(float aAngle, glm::vec3 aRotation);
    void RotateAxis(const glm::vec2& aEulerAxisRotation);
    void RotateAxis(glm::vec3 aEulerRotation);
    void Rotate(glm::quat aRotation);

    void SetScale(glm::vec3 aNewScale);

    void SetMatrix(glm::mat4 aMatrix);

    glm::mat4 GetWorldMatrix() const;
    glm::mat4 GetRotationMatrix() const { return glm::mat4_cast(mRotation); }
    glm::mat4 GetTranslationMatrix() const { return glm::translate(glm::identity<glm::mat4>(), mPosition); }
    glm::mat4 GetScaleMatrix() const { return glm::scale(glm::identity<glm::mat4>(), mScale); }

    void OnImGuiRender() override;
private:
    glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;

    glm::mat4 mWorldMatrix;
};
