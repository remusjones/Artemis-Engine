//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Vertex.h"
#include "glm/vec3.hpp"

// TODO: Merge this with Vertex, and have dynamic descriptors based on assigned components
struct alignas(16) VertexPosition {
    glm::vec3 mPosition;

    VertexPosition() : mPosition(){}
    VertexPosition(const Vertex& vertex) : mPosition(vertex.mPosition) {}


    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexPosition);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>
    GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexPosition, mPosition);
        return attributeDescriptions;
    }
};

