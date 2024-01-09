//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Objects/Entity.h"
#include "Vulkan/Renderers/Renderer.h"

/* Represents a position and a undefined renderer component */
class Primative : public Entity {
public:
    Primative() = default;
    Primative(const char *aEntityName) { mName = aEntityName; }

    void Cleanup() override;

    Renderer *mRenderer;
};
