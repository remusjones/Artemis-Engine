//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "InputManager.h"
#include "Super.h"
#include "Base/Common/Data/GPUCameraData.h"
#include "Math/Transform.h"


class Camera : public Super {
public:
    void Create();

    // Move this to a derived class "FlyCamera" or something.
    void Forward(const KeyboardEvent &keyboardEvent);

    void Backward(const KeyboardEvent &keyboardEvent);

    void Left(const KeyboardEvent &keyboardEvent);

    void Right(const KeyboardEvent &keyboardEvent);

    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);


    void Tick(float aDeltaTime) override;

    glm::mat4 GetViewProjectionMatrix() const;

    glm::mat4 GetPerspectiveMatrix() const;

    glm::mat4 GetViewMatrix() const;

    GPUCameraData GetCameraInformation() const;

    Transform mTransform;

private:
    glm::vec3 mMoveVector = {};
    float_t mSpeed = 10;
    bool mMouseRPressed = false;
};
