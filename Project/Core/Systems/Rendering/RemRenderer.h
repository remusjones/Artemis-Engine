//
// Created by Remus on 15/12/2023.
// Wrapper to create an object that can have a 2d/3d mesh applied to int
//

#pragma once

#include <vector>
#include "Rendering/Data/Vertex.h"

class RemMaterial;
class RemRenderer {
public:

    std::vector<Vertex> m_verticies;
    RemMaterial* m_material;
};
