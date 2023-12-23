//
// Created by Remus on 22/12/2023.
//

#include "Camera.h"

#include "VulkanGraphicsImpl.h"

glm::mat4 Camera::GetPerspectiveMatrix() const {
    //camera projection
    glm::mat4 projection = glm::perspective(glm::radians(70.f),
                                            (float) gGraphics->mSwapChain->
                                            mSwapChainExtent.width / (float)
                                            gGraphics->mSwapChain->
                                            mSwapChainExtent.width, 0.1f,
                                            200.0f);
    return projection * GetViewMatrix();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::translate(glm::mat4(1.f), mTransform.Position());
}

GPUCameraData Camera::GetCameraInformation() const {
    GPUCameraData information{};
    information.mLocationMatrix = mTransform.mTransformationMatrix;
    information.mPerspectiveMatrix = GetPerspectiveMatrix();
    information.mViewMatrix = GetViewMatrix();
    return information;
}
