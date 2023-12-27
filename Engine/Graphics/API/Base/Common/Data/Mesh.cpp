//
// Created by Remus on 20/12/2023.
//

#include "Mesh.h"
#include <tiny_obj_loader.h>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Buffers/AllocatedVertexBuffer.h"
#include "glog/logging.h"

Mesh::Mesh() {
}

Mesh::~Mesh() {
    delete mVertexBuffer;
}

void Mesh::Bind(VkCommandBuffer aCommandBuffer) const {
    const VkBuffer vertexBuffers[] = {mVertexBuffer->mVerticesBuffer->mBuffer};
    const VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(aCommandBuffer, mVertexBuffer->mIndicesBuffer->mBuffer,
                         0, VK_INDEX_TYPE_UINT16);
}

bool Mesh::LoadFromObject(const char *aFileName, const char *aMtlDirectory = "") {
    //attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    //shapes contains the info for each separate object in the file
    std::vector<tinyobj::shape_t> shapes;
    //materials contains the information about the material of each shape, but we won't use it.
    std::vector<tinyobj::material_t> materials;

    //error and warning output from the load function
    std::string warn;
    std::string err;

    //load the OBJ file
    LoadObj(&attrib, &shapes, &materials, &warn, &err, aFileName, aMtlDirectory);

    if (!warn.empty()) {
        LOG(WARNING) << warn;
    }
    //if we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if (!err.empty()) {
        LOG(ERROR) << err;
        return false;
    }


    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            //hardcode loading to triangles
            int fv = 3;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                //copy it into our vertex
                Vertex newVertex;
                newVertex.mPosition.x = vx;
                newVertex.mPosition.y = vy;
                newVertex.mPosition.z = vz;

                newVertex.mNormal.x = nx;
                newVertex.mNormal.y = ny;
                newVertex.mNormal.z = nz;

                //we are setting the vertex color as the vertex normal. This is just for display purposes
                newVertex.mColor = newVertex.mNormal;

                newVertex.mUV.x  = attrib.texcoords[2 * idx.texcoord_index + 0];
                newVertex.mUV.y =  1 - attrib.texcoords[2 * idx.texcoord_index + 1];


                mIndices.push_back(idx.vertex_index);
                mVertices.push_back(newVertex);
            }
            index_offset += fv;
        }
    }
    mVertexBuffer = new AllocatedVertexBuffer(mVertices, mIndices);
    return true;
}
