//
// Created by Remus on 22/12/2023.
//

#include "Camera.h"
#include "VulkanGraphicsImpl.h"

glm::mat4 Camera::GetViewProjectionMatrix() const {
    return GetPerspectiveMatrix() * GetViewMatrix();
}

glm::mat4 Camera::GetPerspectiveMatrix() const {
    const glm::mat4 perspective =
            glm::perspective(glm::radians(mFOV),
                             static_cast<float>(gGraphics->mSwapChain->
                                 mSwapChainExtent.width) / static_cast<float>(gGraphics->mSwapChain->
                                 mSwapChainExtent.width), mZNear,
                             mZFar);

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
