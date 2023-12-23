//
// Created by Remus on 23/12/2023.
//

#pragma once
#include <glm/vec3.hpp>

struct alignas(16) GPULightingData {
    glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f);
    float pad1;
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float pad2;
    // TODO: move these to material properties
    float ambientStrength = 1.0f;
    float specularStrength = 1.0f;
    float shininess = 32.0f;
};
