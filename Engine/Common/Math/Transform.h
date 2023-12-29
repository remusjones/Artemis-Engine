//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Transform {
public:
    Transform();

    glm::vec3 Position() const;
    glm::quat Rotation() const;
    glm::vec3 Euler() const;
    glm::vec3 Scale() const;

    void SetPosition(glm::vec3 aNewPosition);
    void SetRotation(glm::vec3 aNewRotation);
    void SetRotation(glm::mat4 aNewRotation);
    void RotateAround(float aAngle, glm::vec3 aRotation);
    void SetScale(glm::vec3 aNewScale);

    glm::mat4 GetCombinedMatrix() const;
    glm::mat4 GetRotationMatrix() const { return mRotationMatrix; }
    glm::mat4 GetTranslationMatrix() const { return mTranslationMatrix; }
    glm::mat4 GetScaleMatrix() const { return mScaleMatrix; }

private:
    glm::mat4 mTranslationMatrix;
    glm::mat4 mRotationMatrix;
    glm::mat4 mScaleMatrix;
};
