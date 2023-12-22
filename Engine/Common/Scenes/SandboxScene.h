//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "Scene.h"


class SandboxScene final : public Scene {
public:
    void Construct(const char *aSceneName) override;

    void Tick(float aDeltaTime) override;

    void Cleanup() override;

private:
    MeshObject *mMonkey;
    MeshObject *mMonkey2;
};
