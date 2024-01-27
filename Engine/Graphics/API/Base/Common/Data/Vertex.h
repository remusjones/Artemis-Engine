//
// Created by Remus on 15/12/2023.
//

#pragma once

#include <bitset>
#include <vulkan/vulkan_core.h>
#include "glm/glm.hpp"
#include <vector>

enum class VertexAttributeBinding : uint32_t {
    Position = 0,
    Normal,
    Color,
    UV,
    Tangent
};


class alignas(16) Vertex {
public:
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec3 mColor;
    glm::vec2 mUV;
    glm::vec4 mTangent;

    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>
    GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(5);
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

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, mUV);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, mTangent);
        return attributeDescriptions;
    }

    static std::vector<VkVertexInputAttributeDescription>
      GetAttributeDescriptions(std::bitset<5> bindingMask) {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.reserve(bindingMask.count());

        auto addAttributeDescription = [&attributeDescriptions, bindingMask](
            VertexAttributeBinding binding, uint32_t location, VkFormat format, size_t offset) {
            if (bindingMask[static_cast<size_t>(binding)]) {
                VkVertexInputAttributeDescription attributeDescription{};
                attributeDescription.binding = 0;
                attributeDescription.location = location;
                attributeDescription.format = format;
                attributeDescription.offset = static_cast<uint32_t>(offset);
                attributeDescriptions.push_back(attributeDescription);
            }
        };

        addAttributeDescription(VertexAttributeBinding::Position, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mPosition));
        addAttributeDescription(VertexAttributeBinding::Normal, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mNormal));
        addAttributeDescription(VertexAttributeBinding::Color, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mColor));
        addAttributeDescription(VertexAttributeBinding::UV, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mUV));
        addAttributeDescription(VertexAttributeBinding::Tangent, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, mTangent));

        return attributeDescriptions;
    }

    [[nodiscard]] static std::bitset<5> GetBindingMask(
        const bool position = true,
        const bool normal = true,
        const bool color = true,
        const bool uv = true,
        const bool tangent = true) {
        std::bitset<5> bindingMask;

        if (position)
            bindingMask.set(static_cast<size_t>(VertexAttributeBinding::Position));
        if (normal)
            bindingMask.set(static_cast<size_t>(VertexAttributeBinding::Normal));
        if (color)
            bindingMask.set(static_cast<size_t>(VertexAttributeBinding::Color));
        if (uv)
            bindingMask.set(static_cast<size_t>(VertexAttributeBinding::UV));
        if (tangent)
            bindingMask.set(static_cast<size_t>(VertexAttributeBinding::Tangent));

        return bindingMask;
    }
};