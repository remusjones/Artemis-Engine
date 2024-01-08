//
// Created by Remus on 20/12/2023.
//

#include "Mesh.h"
#include <tiny_obj_loader.h>

#include "LoadUtilities.h"
#include "Logger.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    delete mVertexBuffer;
}

void Mesh::Bind(VkCommandBuffer aCommandBuffer) const
{
    const VkBuffer vertexBuffers[] = {mVertexBuffer->mVerticesBuffer->mBuffer};
    const VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mVertexBuffer->mIndicesBuffer->mBuffer,
                         0, VK_INDEX_TYPE_UINT16);
}

bool Mesh::LoadFromObject(const char* aFileName, const char* aMtlDirectory = "")
{
    LoadUtilities::LoadMeshFromDisk(aFileName, &mVertexBuffer, mVertices, mIndices, aMtlDirectory);
    return true;
}
