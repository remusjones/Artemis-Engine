//
// Created by Remus on 9/01/2024.
//

#include "Primative.h"

void Primative::Cleanup() {
    Entity::Cleanup();
    if (mRenderer) {
        mRenderer->DestroyRenderer();
        delete mRenderer;
    }

}