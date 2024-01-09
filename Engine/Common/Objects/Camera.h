//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "Base/Common/Data/GPUCameraData.h"
#include "Entity.h"
#include "Math/Transform.h"


class Camera : public Entity{
public:
    glm::mat4 GetViewProjectionMatrix() const;
    glm::mat4 GetPerspectiveMatrix() const;
    glm::mat4 GetViewMatrix() const;

    GPUCameraData GetCameraInformation() const;

    float_t mFOV = 70.0f;
    float_t mZNear = 0.1f;
    float_t mZFar = 200.0f;
};
