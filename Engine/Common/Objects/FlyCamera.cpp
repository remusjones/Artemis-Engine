//
// Created by Remus on 30/12/2023.
//

#include "FlyCamera.h"

#include <glm/detail/type_quat.hpp>

#include "imgui.h"
#include "InputManager.h"
#include "VulkanGraphicsImpl.h"

void FlyCamera::Construct() {
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
    gInputManager->RegisterKeyCodeInput(SDLK_SPACE,
                                        [&](KeyboardEvent kb) {
                                            Up(kb);
                                        }, "Camera Up");
    gInputManager->RegisterKeyCodeInput(SDLK_LCTRL,
                                        [&](KeyboardEvent kb) {
                                            Down(kb);
                                        }, "Camera Down");

    gInputManager->RegisterMouseInput([&](SDL_MouseMotionEvent motion) { MouseMovement(motion); }, "Camera Mouse");
    gInputManager->RegisterMouseInput([&](SDL_MouseButtonEvent input) { MouseInput(input); }, "Camera Click");
    Entity::Construct();
}

void FlyCamera::OnImGuiRender() {
    ImGui::Indent();
    Entity::OnImGuiRender();
    ImGui::SeparatorText("Settings");
    if (ImGui::DragFloat(GetUniqueLabel("FOV"), &mFOV, 0.1f)) {
    }
    if (ImGui::DragFloat(GetUniqueLabel("Z Near"), &mZNear, 0.01f)) {
    }
    if (ImGui::DragFloat(GetUniqueLabel("Z Far"), &mZFar, 0.1f)) {
    }

    ImGui::SeparatorText("DEBUG");
    if (ImGui::DragFloat3(GetUniqueLabel("Forward"), &m_transform.Forward()[0], 0.1f)) {
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Right"), &m_transform.Right()[0], 0.1f)) {
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Up"), &m_transform.Up()[0], 0.1f)) {
    }
    ImGui::Unindent();
}

void FlyCamera::Forward(const KeyboardEvent &keyboardEvent) {
    mInput[0] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::Backward(const KeyboardEvent &keyboardEvent) {
    mInput[1] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::Left(const KeyboardEvent &keyboardEvent) {
    mInput[2] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::Right(const KeyboardEvent &keyboardEvent) {
    mInput[3] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::Up(const KeyboardEvent &keyboardEvent) {
    mInput[4] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::Down(const KeyboardEvent &keyboardEvent) {
    mInput[5] = keyboardEvent.mPressedState == SDL_PRESSED;
}

void FlyCamera::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    if (!mMouseRPressed)
        return;

    const float xDelta = aMouseMotion.xrel;
    const float yDelta = aMouseMotion.yrel;

    constexpr float sensitivity = 0.1f;
    const glm::vec2 delta = -glm::vec2(yDelta, xDelta) * sensitivity;
    m_transform.RotateAxisLocal(delta);
}

void FlyCamera::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (aMouseInput.button == SDL_BUTTON_RIGHT) {
        mMouseRPressed = aMouseInput.state == SDL_PRESSED;
    }
}

bool FlyCamera::IsCameraConsumingInput() const {
    return mMouseRPressed;
}

glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
    return x * (1.f - t) + y * t;
}

void FlyCamera::Tick(float aDeltaTime) {
    // TODO: investigate global mouse state for off window input
    //float x, y;
    //LOG(INFO) << x << " " << y;
    if (IsCameraConsumingInput()) {
        mMoveVector = glm::vec3();
        mMoveVector.z += mInput[0] ? -mSpeed : 0;
        mMoveVector.z += mInput[1] ? mSpeed : 0;
        mMoveVector.x += mInput[2] ? -mSpeed : 0;
        mMoveVector.x += mInput[3] ? mSpeed : 0;
        mMoveVector.y += mInput[4] ? mSpeed : 0;
        mMoveVector.y += mInput[5] ? -mSpeed : 0;
        m_transform.TranslateLocal(mMoveVector * aDeltaTime);
    } else {
        constexpr float dampeningSpeed = 25.f;
        mMoveVector = lerp(mMoveVector, glm::vec3(), aDeltaTime * dampeningSpeed);
        m_transform.TranslateLocal(mMoveVector * aDeltaTime);
    }


    Entity::Tick(aDeltaTime);
}
