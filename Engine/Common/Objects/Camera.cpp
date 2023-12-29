//
// Created by Remus on 22/12/2023.
//

#include "Camera.h"

#include "VulkanGraphicsImpl.h"

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

void Camera::Tick(float aDeltaTime) {
    Super::Tick(aDeltaTime);
    mTransform.SetPosition(mMoveVector * mSpeed * aDeltaTime + mTransform.Position());
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
    return glm::translate(glm::mat4(1.f), mTransform.Position());
}

GPUCameraData Camera::GetCameraInformation() const {
    GPUCameraData information{};
    information.mViewProjectionMatrix = GetViewProjectionMatrix();
    information.mPerspectiveMatrix = GetPerspectiveMatrix();
    information.mViewMatrix = GetViewMatrix();
    return information;
}
