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

    std::vector<Vertex> GetVertices() { return mVertices; }
    std::vector<int32_t> GetIndices() { return mIndices; }

private:
    std::vector<Vertex> mVertices;
    std::vector<int32_t> mIndices;
    AllocatedVertexBuffer *mVertexBuffer;
};
