//
// Created by Remus on 20/12/2023.
//

#include "Mesh.h"

#include "LoadUtilities.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"

Mesh::Mesh(): mVertexBuffer(nullptr) {
}

Mesh::~Mesh() {
    delete mVertexBuffer;
}

void Mesh::Bind(VkCommandBuffer aCommandBuffer) const {
    const VkBuffer vertexBuffers[] = {mVertexBuffer->mVerticesBuffer->mBuffer};
    const VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mVertexBuffer->mIndicesBuffer->mBuffer,
                         0, VK_INDEX_TYPE_UINT32);
}

bool Mesh::LoadFromObject(const char *aFileName, const char *aMtlDirectory = "") {
    if (LoadUtilities::LoadMeshFromDisk(aFileName, mVertices, mIndices, aMtlDirectory))
    {
        mVertexBuffer = new AllocatedVertexBuffer(mVertices, mIndices);

        std::string bufferName;
        bufferName.append(aFileName);
        vmaSetAllocationName(gGraphics->mAllocator, mVertexBuffer->mVerticesBuffer->mAllocation,
                             (bufferName + " mVerticesBuffer").c_str());

        vmaSetAllocationName(gGraphics->mAllocator, mVertexBuffer->mIndicesBuffer->mAllocation,
                             (bufferName + " mIndicesBuffer").c_str());
        return true;
    }
    return false;
}
