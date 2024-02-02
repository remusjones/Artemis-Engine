//
// Created by Remus on 30/12/2023.
//

#pragma once
#include <SDL_events.h>

#include "Camera.h"


struct KeyboardEvent;

class FlyCamera : public Camera{
public:
    void Construct() override;

    void OnImGuiRender() override;

    void Tick(float aDeltaTime) override;

    void Forward(const KeyboardEvent &keyboardEvent);

    void Backward(const KeyboardEvent &keyboardEvent);

    void Left(const KeyboardEvent &keyboardEvent);

    void Right(const KeyboardEvent &keyboardEvent);

    void Up(const KeyboardEvent &keyboardEvent);

    void Down(const KeyboardEvent &keyboardEvent);

    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);
    bool IsCameraConsumingInput() const;

private:
    glm::vec3 mMoveVector = {};
    bool mInput[6] = {};
    float_t mSpeed = 10;
    bool mMouseRPressed = false;
};
