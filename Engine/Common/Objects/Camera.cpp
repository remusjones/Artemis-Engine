//
// Created by Remus on 22/12/2023.
//

#include "Camera.h"

#include <glm/detail/type_quat.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include "VulkanGraphicsImpl.h"
#include "glog/logging.h"

void Camera::Create() {
    gInputManager->RegisterKeyCodeInput(SDLK_w,
                                        [this](KeyboardEvent kb) {
                                            Forward(kb);
                                        }, "Camera Forward");
    gInputManager->RegisterKeyCodeInput(SDLK_s,
                                        [&](KeyboardEvent kb) {
                                            Backward(kb);
                                        }, "Camera Backward");
    gInputManager->RegisterKeyCodeInput(SDLK_d,
                                        [&](KeyboardEvent kb) {
                                            Right(kb);
                                        }, "Camera Right");
    gInputManager->RegisterKeyCodeInput(SDLK_a,
                                        [&](KeyboardEvent kb) {
                                            Left(kb);
                                        }, "Camera Left");

    gInputManager->RegisterMouseInput([&](SDL_MouseMotionEvent motion) { MouseMovement(motion); });
    gInputManager->RegisterMouseInput([&](SDL_MouseButtonEvent input) { MouseInput(input); });
}


void Camera::Forward(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector[2] = 1;
    else mMoveVector[2] = 0;
}

void Camera::Backward(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector[2] = -1;
    else mMoveVector[2] = 0;
}

void Camera::Left(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector[0] = 1;
    else mMoveVector[0] = 0;
}

void Camera::Right(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector[0] = -1;
    else mMoveVector[0] = 0;
}

float oldX;
float oldY;
float lastDt;

void Camera::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    if (!mMouseRPressed)
        return;

    float xDelta = aMouseMotion.xrel;
    float yDelta = aMouseMotion.yrel;

    float sensitivity = 0.1f;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(xDelta * sensitivity),
                                           glm::vec3(0.0f, 1.0f, 0.0f))
                               * glm::rotate(glm::mat4(1.0f), glm::radians(-yDelta * sensitivity),
                                             glm::vec3(1.0f, 0.0f, 0.0f));

    mTransform.SetRotation(mTransform.GetRotationMatrix() * rotationMatrix);
}

void Camera::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (aMouseInput.button == SDL_BUTTON_RIGHT) {
        mMouseRPressed = aMouseInput.state == SDL_PRESSED;
    }
}


void Camera::Tick(float aDeltaTime) {

    // TODO: investigate global mouse state for off window input
    //float x, y;
    //SDL_GetGlobalMouseState(&x, &y);
    //LOG(INFO) << x << " " << y;
    Super::Tick(aDeltaTime);
    if (mMoveVector.length() > 0){
        // TODO: Use relative direction
        mTransform.SetPosition(mMoveVector * mSpeed * aDeltaTime + mTransform.Position());
    }
}


glm::mat4 Camera::GetViewProjectionMatrix() const {
    return GetPerspectiveMatrix() * GetViewMatrix();
}

glm::mat4 Camera::GetPerspectiveMatrix() const {
    const glm::mat4 perspective =
            glm::perspective(glm::radians(70.f),
                             static_cast<float>(gGraphics->mSwapChain->
                                 mSwapChainExtent.width) / static_cast<float>(gGraphics->mSwapChain->
                                 mSwapChainExtent.width), 0.1f,
                             200.0f);

    return perspective;
}

glm::mat4 Camera::GetViewMatrix() const {
    return mTransform.GetRotationMatrix() * mTransform.GetTranslationMatrix();
}

GPUCameraData Camera::GetCameraInformation() const {
    GPUCameraData information{};
    information.mViewProjectionMatrix = GetViewProjectionMatrix();
    information.mPerspectiveMatrix = GetPerspectiveMatrix();
    information.mViewMatrix = GetViewMatrix();
    return information;
}
