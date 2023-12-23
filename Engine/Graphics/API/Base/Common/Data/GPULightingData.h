//
// Created by Remus on 23/12/2023.
//

#pragma once
#include <glm/vec3.hpp>

struct GPULightingData {
    glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    // TODO: move these to material properties
    float ambientStrength = 0.1f;
    float specularStrength = 0.5f;
    float shininess = 32.0f;
};
