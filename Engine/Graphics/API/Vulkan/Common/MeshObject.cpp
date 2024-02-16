//
// Created by Remus on 17/12/2023.
//

#include "MeshObject.h"
#include <Logger.h>
#include "Base/Common/Material.h"
#include "Scenes/Scene.h"

void MeshObject::Construct() {
    Entity::Construct();
}

void MeshObject::Tick(float aDeltaTime) {
    Entity::Tick(aDeltaTime);
}

void MeshObject::Cleanup() {
    mMeshRenderer.DestroyRenderer();
    Entity::Cleanup();
}

void MeshObject::CreateObject(
    Material &aMaterial,
    const char *aName) {
    mName = aName;
    Logger::Log(spdlog::level::info, (std::string("Creating object ") + mName).c_str());
    mMeshRenderer.mMaterial = &aMaterial;
    mMeshRenderer.mTransform = &mTransform;
}

void MeshObject::OnImGuiRender() {
    Entity::OnImGuiRender();
    mMeshRenderer.mMaterial->OnImGuiRender();
}