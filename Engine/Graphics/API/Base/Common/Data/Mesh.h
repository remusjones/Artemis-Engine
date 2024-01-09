//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <cstdint>
#include <vector>
#include "Vertex.h"

class AllocatedVertexBuffer;

class Mesh {
public:
    Mesh();
    ~Mesh();

    void Bind(VkCommandBuffer aCommandBuffer) const;
    bool LoadFromObject(const char *aFileName, const char *aMtlDirectory);

    [[nodiscard]] std::vector<Vertex> GetVertices() const { return mVertices; }
    [[nodiscard]] std::vector<int32_t> GetIndices() const { return mIndices; }

    [[nodiscard]] int32_t GetVerticesSize() const { return mVertices.size(); }
    [[nodiscard]] int32_t GetIndicesSize() const { return mIndices.size(); }

private:
    std::vector<Vertex> mVertices;
    std::vector<int32_t> mIndices;
    AllocatedVertexBuffer *mVertexBuffer;
};
