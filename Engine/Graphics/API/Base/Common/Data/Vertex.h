//
// Created by Remus on 15/12/2023.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include <array>

#include "Base/Common/Buffers/AllocatedVertexBuffer.h"
#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec3 mColor;

    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, mPosition);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, mNormal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, mColor);

        return attributeDescriptions;
    }
};

struct Mesh {
    std::vector<Vertex> mVertices;
    std::vector<int16_t> mIndices;
    AllocatedVertexBuffer mVertexBuffer;
};