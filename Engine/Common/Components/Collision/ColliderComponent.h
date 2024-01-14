//
// Created by Remus on 13/01/2024.
//

#pragma once
#include "Objects/Component.h"

class btCollisionShape;

class ColliderComponent : public Component{
private:
    btCollisionShape* mCollisionShape;
};
