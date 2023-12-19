//
// Created by remus on 19/12/2023.
//
#pragma once

#include <vector>
#include <cstdint>

class Buffer;
class Vertex;
class VertexBuffer {
public:
    VertexBuffer(std::vector<Vertex> aVertices, std::vector<int16_t> aIndices);
    ~VertexBuffer();

    Buffer* mVerticesBuffer;
    Buffer* mIndicesBuffer;
};
