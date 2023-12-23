//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "Base/Common/Data/GPUCameraData.h"
#include "Math/Transform.h"


class Camera {
public:

    glm::mat4 GetPerspectiveMatrix() const;
    glm::mat4 GetViewMatrix() const;
    GPUCameraData GetCameraInformation() const;

    Transform mTransform;
};
