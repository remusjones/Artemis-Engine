//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


class Transform {
public:
    Transform();

    glm::vec3 Position() const;
    glm::quat Rotation() const;
    glm::vec3 Euler() const;
    glm::vec3 Scale() const;

    void SetPosition(glm::vec3 aNewPosition);
    void SetRotation(glm::vec3 aEulerRotation);
    void SetRotation(glm::quat aNewRotation);
    void Rotate(glm::quat aRotation);

    void RotateAround(float aAngle, glm::vec3 aRotation);

    void SetScale(glm::vec3 aNewScale);

    glm::mat4 GetCombinedMatrix() const;

    glm::mat4 GetRotationMatrix() const { return glm::mat4_cast(mRotation); }
    glm::mat4 GetTranslationMatrix() const { return glm::translate(glm::identity<glm::mat4>(), mPosition); }
    glm::mat4 GetScaleMatrix() const { return glm::scale(glm::identity<glm::mat4>(), mScale); }

private:
    glm::vec3 mPosition;
    glm::quat mRotation;
    glm::vec3 mScale;

    glm::mat4 mWorldMatrix;
};
