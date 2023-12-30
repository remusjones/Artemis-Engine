//
// Created by Remus on 30/12/2023.
//

#pragma once
#include <SDL_events.h>

#include "Camera.h"
#include "ImGuiLayer.h"
#include "Super.h"


struct KeyboardEvent;

class FlyCamera : public Camera, ImGuiLayer, Super {
public:
    void Construct() override;
    void OnImGuiRender() override;
    void Tick(float aDeltaTime) override;

    void Forward(const KeyboardEvent &keyboardEvent);

    void Backward(const KeyboardEvent &keyboardEvent);

    void Left(const KeyboardEvent &keyboardEvent);

    void Right(const KeyboardEvent &keyboardEvent);

    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);

private:
    glm::vec3 mMoveVector = {};

    float_t mSpeed = 10;
    bool mMouseRPressed = false;
};
