//
// Created by Remus on 13/01/2024.
//

#pragma once
#include <string>


class Entity;

class Component {
public:
    virtual ~Component() = default;

    virtual void SetEntity(Entity* aEntity);
    virtual void ClearEntity();

    virtual void SetName(const std::string &aName);
    virtual std::string GetName();

    virtual void Tick(float aDeltaTime);
    virtual void Destroy();
    virtual void Initialize();


private:
    Entity* mAttachedEntity;

    // TODO: Replace with Hash
    std::string mName;
};
