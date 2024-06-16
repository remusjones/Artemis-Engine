//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"

#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <Logger.h>
#include "imgui.h"
#include "ImGuizmo.h"

#include "VulkanGraphicsImpl.h"
#include <FileManagement.h>

#include "Profiler.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Components/Component.h"
#include "Components/Collision/ColliderComponent.h"
#include "Components/Collision/CollisionHelper.h"
#include "glm/gtx/string_cast.hpp"
#include "Physics/PhysicsSystem.h"
#include "Physics/Ray.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Systems/GraphicsPipeline.h"
#include "Vulkan/Systems/RenderSystemBase.h"


static ImGuizmo::MODE currentGizmoMode(ImGuizmo::WORLD);
static ImGuizmo::OPERATION currentGizmoOperation(ImGuizmo::TRANSLATE);

void Scene::PreConstruct(const char *aSceneName) {
    PROFILE_BEGIN("SCENE CONSTRUCT");
    m_sceneName = aSceneName;

    m_physicsSystem = new PhysicsSystem();
    m_SceneInteractionPhysicsSystem = new PhysicsSystem();

    m_physicsSystem->Create();
    m_SceneInteractionPhysicsSystem->Create();
    gInputManager->RegisterMouseInput([&](const SDL_MouseMotionEvent &motion) { MouseMovement(motion); },
                                      "Scene Mouse Movement");
    gInputManager->RegisterMouseInput([&](const SDL_MouseButtonEvent &input) { MouseInput(input); },
                                      "Scene Mouse Press");

    gInputManager->RegisterKeyCodeInput(SDLK_w,
                                        [this](KeyboardEvent) {
                                            if (!m_ActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::TRANSLATE);
                                        }, "Scene Gizmo Translate");

    gInputManager->RegisterKeyCodeInput(SDLK_e,
                                        [this](KeyboardEvent) {
                                            if (!m_ActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::ROTATE);
                                        }, "Scene Gizmo Rotate");

    gInputManager->RegisterKeyCodeInput(SDLK_r,
                                        [this](KeyboardEvent) {
                                            if (!m_ActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::SCALE);
                                        }, "Scene Gizmo Scale");
    PROFILE_END();
}


void Scene::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    m_mouseX = static_cast<int>(aMouseMotion.x);
    m_mouseY = static_cast<int>(aMouseMotion.y);
}

void Scene::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (m_ActiveSceneCamera->IsCameraConsumingInput())
        return;

    if (aMouseInput.button == SDL_BUTTON_LEFT
        && aMouseInput.state == SDL_PRESSED
        && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
        && !ImGuizmo::IsOver()) {
        const auto pickedRigidBody = PickRigidBody(m_mouseX, m_mouseY);
        for (const auto object: mObjects) {
            if (ColliderComponent comp; object->GetComponent<ColliderComponent>(comp)
                                        && pickedRigidBody == comp.GetRigidBody()) {
                m_PickedEntity = object;
                break;
            }
        }
    }
}

void Scene::Construct() {
    for (const auto obj: mObjects) {
        obj->Construct();
    }
    assert(m_ActiveSceneCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame) {
    for (const auto obj: mRenderSystems) {
        obj->m_graphicsPipeline->Draw(aCommandBuffer, *this);
    }
}


void Scene::DrawObjectsRecursive(Entity *obj) {
    const char *nodeLabel = obj->GetUniqueLabel(obj->mName);

    if (IsParentOfPickedEntity(obj)) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }

    ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (obj->m_transform.GetChildCount() == 0)
        nodeFlag = ImGuiTreeNodeFlags_Leaf;

    if (m_PickedEntity == obj) {
        nodeFlag |= ImGuiTreeNodeFlags_Selected;
        ImGui::Begin("Picked Object");
        ImGui::Text(m_PickedEntity->mName);
        obj->OnImGuiRender();
        ImGui::End();
    }

    if (ImGui::TreeNodeEx(nodeLabel, nodeFlag)) {
        for (auto childTransform: obj->m_transform.GetChildren()) {
            Entity *childEntity = mTransformEntityRelationships[childTransform];
            DrawObjectsRecursive(childEntity);
        }
        ImGui::TreePop();
    }

    if (ImGui::IsItemClicked()) {
        m_PickedEntity = obj;
    }
}

bool Scene::IsParentOfPickedEntity(const Entity *obj) {
    if (m_PickedEntity == nullptr) {
        return false;
    }
    for (const Entity *parentEntity = m_PickedEntity; parentEntity != nullptr;
         parentEntity = mTransformEntityRelationships[parentEntity->m_transform.GetParent()]) {
        if (parentEntity == obj) {
            return true;
        }
    }
    return false;
}

void Scene::ChangeImGuizmoOperation(const int aOperation) {
    currentGizmoOperation = static_cast<ImGuizmo::OPERATION>(aOperation);
}

void Scene::OnImGuiRender() {
    ImGui::Begin(m_sceneName);

    const ImGuiIO &io = ImGui::GetIO();
    // Draw Gizmo Controls TODO: Add KB Control shortcuts
    ImGui::SeparatorText("Controls");
    ImGui::BeginChild(GetUniqueLabel("Controls"),
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY); {
        if (ImGui::RadioButton(GetUniqueLabel("Translate"), currentGizmoOperation == ImGuizmo::TRANSLATE))
            currentGizmoOperation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Rotate"), currentGizmoOperation == ImGuizmo::ROTATE))
            currentGizmoOperation = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Scale"), currentGizmoOperation == ImGuizmo::SCALE))
            currentGizmoOperation = ImGuizmo::SCALE;

        if (currentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton(GetUniqueLabel("Local"), currentGizmoMode == ImGuizmo::LOCAL))
                currentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton(GetUniqueLabel("World"), currentGizmoMode == ImGuizmo::WORLD))
                currentGizmoMode = ImGuizmo::WORLD;
        }
    }
    ImGui::EndChild();

    if (ImGui::CollapsingHeader("Scene Information")) {
        ImGui::Indent();
        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::ColorEdit3(GetUniqueLabel("Point Light Color"), &mSceneData.color[0]);
            ImGui::DragFloat(GetUniqueLabel("Point Light Intensity"), &mSceneData.lightIntensity, 0.0125f);
            ImGui::DragFloat(GetUniqueLabel("Ambient Lighting"), &mSceneData.ambientStrength, 0.1f);
        }
        if (ImGui::CollapsingHeader("Camera")) {
            m_ActiveSceneCamera->OnImGuiRender();
        }
        ImGui::Unindent();
    }

    if (m_PickedEntity != nullptr) {
        // Draw Gizmos
        glm::mat4 matrix = m_PickedEntity->m_transform.GetWorldMatrix();

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::AllowAxisFlip(false);
        if (ImGuizmo::Manipulate(glm::value_ptr(m_ActiveSceneCamera->GetViewMatrix()),
                                 glm::value_ptr(m_ActiveSceneCamera->GetPerspectiveMatrix()),
                                 currentGizmoOperation, currentGizmoMode,
                                 glm::value_ptr(matrix), NULL)) {
            m_PickedEntity->m_transform.SetMatrix(matrix);
        }
    }

    if (ImGui::CollapsingHeader("Objects")) {
        for (const auto obj: mObjects) {
            ImGui::BeginGroup();
            ImGui::Indent(); {
                if (obj->m_transform.GetParent() == nullptr) {
                    DrawObjectsRecursive(obj);
                }
            }
            ImGui::Unindent();
            ImGui::EndGroup();
        }
    }


    ImGui::SeparatorText("Statistics");
    ImGui::BeginChild(GetUniqueLabel("Statistics"),
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY); {
        ImGui::Text("FPS: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(gGraphics->GetFps())).c_str());
        ImGui::SameLine();
        ImGui::PlotLines("##fpsHistory", &gGraphics->GetFpsHistory().mBuffer.front(),
                         gGraphics->GetFpsHistory().mBuffer.size());
        ImGui::Text("Delta Time: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(gGraphics->DeltaTimeUnscaled()).c_str());

        ImGui::Text("Objects: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(mObjects.size())).c_str());
        ImGui::Text("Graphic Systems: ");
        ImGui::SameLine();
        ImGui::Text(std::to_string(static_cast<int32_t>(mRenderSystems.size())).c_str());
        if (ImGui::CollapsingHeader("Graphic Systems Info")) {
            if (ImGui::Button(GetUniqueLabel("Rebuild All"))) {
                gGraphics->mVulkanEngine.SubmitEndOfFrameTask([this] {
                    for (const auto& renderSystem: mRenderSystems) {
                        vkDeviceWaitIdle(gGraphics->mLogicalDevice);
                        renderSystem->m_graphicsPipeline->Destroy();
                        renderSystem->Create(renderSystem->GetBoundDescriptors());
                        renderSystem->m_graphicsPipeline->Create();
                    }
                });
            }
            ImGui::Indent();
            for (const auto system: mRenderSystems) {
                if (ImGui::CollapsingHeader(system->m_graphicsPipeline->mPipelineName)) {
                    ImGui::Indent();
                    ImGui::Text("Material Count: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(system->m_graphicsPipeline->mRenderers.size()).c_str());
                    for (const auto renderer: system->m_graphicsPipeline->mRenderers) {
                        ImGui::Text(renderer->mMaterial->mMaterialName);
                    }
                    ImGui::Unindent();
                }
            }
            ImGui::Unindent();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void Scene::Tick(const float aDeltaTime) {
    PROFILE_BEGIN("Scene Physics");
    m_physicsSystem->Tick(aDeltaTime);
    m_SceneInteractionPhysicsSystem->Tick(aDeltaTime);
    for (const auto obj: mObjects) {
        obj->Tick(aDeltaTime);
    }
    PROFILE_END();
}

void Scene::Cleanup() {
    Logger::Log(spdlog::level::info, (std::string("Cleaning up scene ") + m_sceneName).c_str());
    for (const auto obj: mObjects) {
        obj->Cleanup();
        delete obj;
    }

    delete m_ActiveSceneCamera;
    for (const auto system: mRenderSystems) {
        system->m_graphicsPipeline->Destroy();
        delete system->m_graphicsPipeline.get();
    }
    m_physicsSystem->Destroy();
    delete m_physicsSystem;
    m_physicsSystem = nullptr;

    m_SceneInteractionPhysicsSystem->Destroy();
    delete m_SceneInteractionPhysicsSystem;
    m_SceneInteractionPhysicsSystem = nullptr;
}

void Scene::AddRenderSystem(RenderSystemBase *aRenderSystem) {
    mRenderSystems.push_back(aRenderSystem);
    aRenderSystem->m_graphicsPipeline->Create();
}

// TODO: Make pointers managed
MeshObject *Scene::CreateObject(const char *aName, const char *aMeshPath, Material &aMaterial,
                                GraphicsPipeline &aPipeline, const glm::vec3 aPos, const glm::vec3 aRot,
                                const glm::vec3 aScale) {
    auto *object = new MeshObject();

    object->CreateObject(aMaterial, aName);
    object->mMeshRenderer.BindRenderer(aPipeline);
    object->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() + aMeshPath).c_str());

    object->m_transform.SetLocalPosition(aPos);
    object->m_transform.SetLocalRotation(aRot);
    object->m_transform.SetLocalScale(aScale);

    //auto *sceneCollider = new ColliderComponent();
    //sceneCollider->SetName("SceneCollider");
    //ColliderCreateInfo colliderInfo;
    //colliderInfo.collisionShape = CollisionHelper::MakeCollisionMesh(object->mMeshRenderer.mMesh->GetVertices(),
    //                                                                  object->mMeshRenderer.mMesh->GetIndices());
    ////   colliderInfo.collisionShape = CollisionHelper::MakeAABBCollision(object->mMeshRenderer.mMesh->GetVertices());
    //sceneCollider->Create(m_SceneInteractionPhysicsSystem, colliderInfo);
    //object->AddComponent(sceneCollider);

    AddEntity(object);
    return object;
}

void Scene::AddEntity(Entity *aEntity) {
    mObjects.push_back(aEntity);
    mTransformEntityRelationships[&aEntity->m_transform] = aEntity;
}

void Scene::AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass,
                                 float aFriction) const {
    auto *sphereCollider = new ColliderComponent();
    ColliderCreateInfo sphereColliderInfo{};
    sphereColliderInfo.collisionShape = new btSphereShape(aRadius);
    sphereColliderInfo.mass = aMass;
    sphereColliderInfo.friction = aFriction;
    sphereCollider->Create(m_physicsSystem, sphereColliderInfo);
    aEntity.AddComponent(sphereCollider);
}

void Scene::AttachBoxCollider(Entity &aEntity, const glm::vec3 aHalfExtents, const float aMass,
                              const float aFriction) const {
    auto *boxCollider = new ColliderComponent();
    ColliderCreateInfo boxColliderInfo{};
    boxColliderInfo.collisionShape = new btBoxShape(btVector3(aHalfExtents.x, aHalfExtents.y, aHalfExtents.z));
    boxColliderInfo.mass = aMass;
    boxColliderInfo.friction = aFriction;
    boxCollider->Create(m_physicsSystem, boxColliderInfo);
    aEntity.AddComponent(boxCollider);
}

const btRigidBody *Scene::PickRigidBody(const int x, const int y) const {
    const Ray ray = m_ActiveSceneCamera->GetRayTo(x, y);
    const btVector3 rayFrom(CollisionHelper::GlmToBullet(ray.origin));
    const btVector3 rayTo(CollisionHelper::GlmToBullet(ray.origin + ray.direction * m_ActiveSceneCamera->mZFar));

    btCollisionWorld::ClosestRayResultCallback RayCallback(rayFrom, rayTo);

    m_physicsSystem->GetDynamicsWorld()->rayTest(rayFrom, rayTo, RayCallback);
    if (RayCallback.hasHit()) {
        if (const btRigidBody *pickedBody = btRigidBody::upcast(RayCallback.m_collisionObject)) {
            return pickedBody;
        }
    }

    return nullptr;
}

Texture *Scene::CreateTexture(const char *aName, std::vector<std::string> aPathsSet) {
    if (!mLoadedTextures.contains(aName)) {
        mLoadedTextures[aName] = std::make_unique<Texture>();

        auto *texture = mLoadedTextures[aName].get();
        texture->LoadImagesFromDisk(aPathsSet);
        texture->Create();

        return texture;
    }

    Logger::Log(spdlog::level::err, "Texture Already Exists");
    return nullptr;
}
