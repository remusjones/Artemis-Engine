//
// Created by Remus on 23/12/2023.
//

#pragma once
#include <glm/vec3.hpp>

struct alignas(16) GPUSceneData {
    glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f);
    float lightIntensity = 1;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientStrength = 0.0f;
};
