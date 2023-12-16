//
// Created by Remus on 15/12/2023.
// Wrapper to create an object that can have a 2d/3d mesh applied to int
//

#pragma once

#include <vector>
#include "Wrappers/Data/Vertex.h"

class VulkanMaterial;
class RemRenderer {
public:

    std::vector<Vertex> m_verticies;
    VulkanMaterial* m_material;
};
