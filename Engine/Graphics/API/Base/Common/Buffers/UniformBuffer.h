//
// Created by remus on 18/12/2023.
//
#pragma once

#include <vector>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "glm/glm.hpp"
#include "Base/Common/Buffers/Buffer.h"

class UniformBufferObject{
public:
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;
};

class UniformBuffer{
public:

    UniformBuffer();
    ~UniformBuffer();

    std::vector<Buffer*> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;
};