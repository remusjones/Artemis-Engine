//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <vector>

#include "VertexPosition.h"

class AllocatedBuffer;

class PrimativeMesh {
public:
    void Bind(VkCommandBuffer aCommandBuffer) const;

    bool LoadFromObject(const char *aFileName, const char *mtlDirectory);

    ~PrimativeMesh();

    [[nodiscard]] std::vector<VertexPosition> GetVertices() const { return mVertices; }
    [[nodiscard]] std::vector<int32_t> GetIndices() const { return mIndices; }

    [[nodiscard]] int32_t GetVerticesSize() const { return mVertices.size(); }
    [[nodiscard]] int32_t GetIndicesSize() const { return mIndices.size(); }

private:
    std::vector<VertexPosition> mVertices;
    std::vector<int32_t> mIndices;
    AllocatedBuffer *mVerticesBuffer;
    AllocatedBuffer *mIndicesBuffer;
};
