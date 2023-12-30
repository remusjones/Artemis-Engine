//
// Created by Remus on 30/12/2023.
//

#include "FlyCamera.h"

#include <glm/detail/type_quat.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include "imgui.h"
#include "InputManager.h"

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

    gInputManager->RegisterMouseInput([&](SDL_MouseMotionEvent motion) { MouseMovement(motion); }, "Camera Mouse");
    gInputManager->RegisterMouseInput([&](SDL_MouseButtonEvent input) { MouseInput(input); }, "Camera Click");
    Super::Construct();
}

void FlyCamera::OnImGuiRender() {
    ImGui::Indent();

    ImGui::SeparatorText("Transform");
    glm::vec3 pos = mTransform.Position();
    glm::vec3 rot = mTransform.Euler();
    glm::vec3 scale = mTransform.Scale();

    if (ImGui::DragFloat3(GetUniqueLabel("Position"), &pos[0], 0.1f)) {
        mTransform.SetPosition(pos);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Rotation"), &rot[0], 0.1f)) {
        mTransform.SetRotation(rot);
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Scale"), &scale[0], 0.1f)) {
        mTransform.SetScale(scale);
    }

    ImGui::SeparatorText("Settings");
    if (ImGui::DragFloat(GetUniqueLabel("FOV"), &mFOV, 0.1f)) {
    }
    if (ImGui::DragFloat(GetUniqueLabel("Z Near"), &mZNear, 0.01f)) {
    }
    if (ImGui::DragFloat(GetUniqueLabel("Z Far"), &mZFar, 0.1f)) {
    }

    ImGui::SeparatorText("DEBUG");
    if (ImGui::DragFloat3(GetUniqueLabel("Transform Forward"), &mTransform.Forward()[0], 0.1f)) {
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Transform Right"), &mTransform.Right()[0], 0.1f)) {
    }
    if (ImGui::DragFloat3(GetUniqueLabel("Transform Up"), &mTransform.Up()[0], 0.1f)) {
    }

    ImGui::Unindent();
}

void FlyCamera::Forward(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector.z = mSpeed;
    else mMoveVector.z = 0;
}

void FlyCamera::Backward(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector.z = -mSpeed;
    else mMoveVector.z = 0;
}

void FlyCamera::Left(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector.x = mSpeed;
    else mMoveVector.x = 0;
}

void FlyCamera::Right(const KeyboardEvent &keyboardEvent) {
    if (keyboardEvent.mPressedState == SDL_PRESSED)
        mMoveVector.x = -mSpeed;
    else mMoveVector.x = 0;
}

void FlyCamera::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    if (!mMouseRPressed)
        return;

    float xDelta = aMouseMotion.xrel;
    float yDelta = aMouseMotion.yrel;

    float sensitivity = 0.1f;

    //TODO: Consider Forward direction here
    const glm::vec2 axisRotation = glm::vec2(-yDelta * sensitivity, xDelta * sensitivity);
    mTransform.RotateAxis(axisRotation);
}

void FlyCamera::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (aMouseInput.button == SDL_BUTTON_RIGHT) {
        mMouseRPressed = aMouseInput.state == SDL_PRESSED;
    }
}


void FlyCamera::Tick(float aDeltaTime) {
    // TODO: investigate global mouse state for off window input
    //float x, y;
    //LOG(INFO) << x << " " << y;

    Super::Tick(aDeltaTime);
    const glm::vec3 rightMoveVec = glm::vec3(mMoveVector.x * mTransform.Forward());
    const glm::vec3 fwdMoveVec = glm::vec3(mMoveVector.z * mTransform.Right());
    const glm::vec3 final = glm::vec3(fwdMoveVec.z + rightMoveVec.z, 0, fwdMoveVec.x + rightMoveVec.x) * 0.5f;

    mTransform.Translate(final * aDeltaTime);
}
