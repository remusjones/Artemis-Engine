//
// Created by Remus on 27/12/2023.
//

#pragma once
#include "VulkanGraphicsImpl.h"


class Mesh;
class AllocatedImage;

class LoadUtilities {
public:
    static bool LoadImageFromDisk(VulkanGraphics *aEngine, const char *aFilePath, AllocatedImage &aResult);

    static bool LoadMeshFromDisk(const char *aFilePath, AllocatedVertexBuffer &aResult,
                                 std::vector<Vertex> &aResultVertices,
                                 std::vector<int16_t> &aResultIndices);

};
