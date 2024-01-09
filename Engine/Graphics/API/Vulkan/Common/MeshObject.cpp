//
// Created by Remus on 17/12/2023.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MeshObject.h"
#include "imgui.h"
#include "Logger.h"
#include "Base/Common/Material.h"
#include "Scenes/Scene.h"
#include <glm/gtc/type_ptr.hpp>

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