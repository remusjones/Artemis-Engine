//
// Created by Remus on 20/12/2023.
//

#include "AllocatedVertexBuffer.h"

#include "AllocatedBuffer.h"
#include "Logger.h"
#include "Base/Common/Data/Vertex.h"

AllocatedVertexBuffer::AllocatedVertexBuffer(const std::vector<Vertex> &aVertices,
                                             const std::vector<int32_t> &aIndices) {
    mVerticesBuffer = new AllocatedBuffer(aVertices.data(),
                                          sizeof(aVertices[0]) *
                                          aVertices.size(),
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    mIndicesBuffer = new AllocatedBuffer(aIndices.data(),
                                         sizeof(aIndices[0]) * aIndices.size(),
                                         VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

AllocatedVertexBuffer::~AllocatedVertexBuffer() {
    mVerticesBuffer->Destroy();
    delete mVerticesBuffer;
    mIndicesBuffer->Destroy();
    delete mIndicesBuffer;
}
