//
// Created by Remus on 9/01/2024.
//

#include "PrimativeMesh.h"

#include "LoadUtilities.h"

void PrimativeMesh::Bind(VkCommandBuffer aCommandBuffer) const {
    const VkBuffer vertexBuffers[] = {mVerticesBuffer->GetBuffer()};
    const VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mIndicesBuffer->GetBuffer(),
                         0, VK_INDEX_TYPE_UINT32);
}

bool PrimativeMesh::LoadFromObject(const char *aFileName, const char *aMtlDirectory) {
    std::vector<Vertex> vertices;

    if (LoadUtilities::LoadMeshFromDisk(aFileName, vertices, mIndices, aMtlDirectory)) {
        mVertices = std::vector<Vertex>(vertices.size());
        for (int i = 0; i < vertices.size(); i++) {
            mVertices[i] = vertices[i];
        }
        mVerticesBuffer = new AllocatedBuffer(mVertices.data(), (sizeof(mVertices[0]) + 1) * mVertices.size(),
                                              VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        mIndicesBuffer = new AllocatedBuffer(mIndices.data(), (sizeof(mIndices[0]) + 1) *mIndices.size(),
                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        vmaSetAllocationName(gGraphics->mAllocator, mVerticesBuffer->GetAllocation(),
                             (std::string(aFileName) + " mVerticesBuffer").c_str());
        vmaSetAllocationName(gGraphics->mAllocator, mIndicesBuffer->GetAllocation(),
                             (std::string(aFileName) + " mIndicesBuffer").c_str());
        return true;
    }
    return false;
}

PrimativeMesh::~PrimativeMesh() {
    if (mVerticesBuffer) {
        mVerticesBuffer->Destroy();
        delete mVerticesBuffer;
    }
    if (mIndicesBuffer) {
        mIndicesBuffer->Destroy();
        delete mIndicesBuffer;
    }
}
