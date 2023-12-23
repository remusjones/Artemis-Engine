//
// Created by Remus on 20/12/2023.
//

#pragma once
#include <vector>
#include <vk_mem_alloc.h>

class AllocatedBuffer;
class Vertex;

class AllocatedVertexBuffer {
public:

    AllocatedVertexBuffer(const std::vector<Vertex> &aVertices, const std::vector<int16_t>
                          &aIndices);
    ~AllocatedVertexBuffer();

    AllocatedBuffer* mVerticesBuffer;
    AllocatedBuffer* mIndicesBuffer;
};