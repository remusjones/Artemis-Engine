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
    if (LoadUtilities::LoadMeshFromDisk(aFileName, mVertices, mIndices, aMtlDirectory)) {
        CalculateTangents(mVertices, mIndices);
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

void Mesh::CalculateTangents(std::vector<Vertex> &vertices, const std::vector<int32_t> &indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        int index0 = indices[i];
        int index1 = indices[i + 1];
        int index2 = indices[i + 2];

        glm::vec3 &v0 = vertices[index0].mPosition;
        glm::vec3 &v1 = vertices[index1].mPosition;
        glm::vec3 &v2 = vertices[index2].mPosition;

        glm::vec2 &uv0 = vertices[index0].mUV;
        glm::vec2 &uv1 = vertices[index1].mUV;
        glm::vec2 &uv2 = vertices[index2].mUV;

        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        const glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vertices[index0].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
        vertices[index1].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
        vertices[index2].mTangent = glm::vec4(tangent.x, tangent.y, tangent.z, 0);
    }
}
