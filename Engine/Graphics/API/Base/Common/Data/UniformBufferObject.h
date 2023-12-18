//
// Created by remus on 18/12/2023.
//
#pragma once

#include "glm/ext/matrix_float4x4.hpp"

class UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};