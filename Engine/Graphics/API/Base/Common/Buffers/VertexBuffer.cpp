//
// Created by remus on 19/12/2023.
//

#include "VertexBuffer.h"
#include "Buffer.h"
#include "../Data/Vertex.h"

VertexBuffer::VertexBuffer(const std::vector<Vertex> aVertices, const std::vector<int16_t> aIndices) {

    mVerticesBuffer = new Buffer(aVertices.data(), sizeof(aVertices[0]) * aVertices.size(),
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    mIndicesBuffer = new Buffer(aIndices.data(), sizeof(aIndices[0]) * aIndices.size(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

VertexBuffer::~VertexBuffer() {
    delete mVerticesBuffer;
    delete mIndicesBuffer;
}
