//
// Created by Remus on 15/12/2023.
// Wrapper to create an object that can have a 2d/3d mesh applied to int
//

#pragma once

#include <vector>
#include "API/Base/Wrappers/Renderer.h"
#include "API/Base/Wrappers/Data/Vertex.h"

class Material;
class VulkanRenderer : Renderer{
public:

    std::vector<Vertex> vertices;
    Material* material;
};
