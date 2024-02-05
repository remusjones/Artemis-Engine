//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"

#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "ImGuizmo.h"
#include "Logger.h"

#include "VulkanGraphicsImpl.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Components/Collision/ColliderComponent.h"
#include "Components/Collision/CollisionHelper.h"
#include "../../IO/FileManagement.h"
#include "Physics/PhysicsSystem.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Systems/GraphicsPipeline.h"
#include "Components/Component.h"
#include "Physics/Ray.h"
#include "glm/gtx/string_cast.hpp"
#include "Vulkan/Systems/RenderSystemBase.h"


static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void Scene::PreConstruct(const char *aSceneName) {
    mSceneName = aSceneName;

    mPhysicsSystem = new PhysicsSystem();
    mSceneInteractionPhysicsSystem = new PhysicsSystem();

    mPhysicsSystem->Create();
    mSceneInteractionPhysicsSystem->Create();
    gInputManager->RegisterMouseInput([&](const SDL_MouseMotionEvent &motion) { MouseMovement(motion); },
                                      "Scene Mouse Movement");
    gInputManager->RegisterMouseInput([&](const SDL_MouseButtonEvent &input) { MouseInput(input); },
                                      "Scene Mouse Press");

    gInputManager->RegisterKeyCodeInput(SDLK_w,
                                        [this](KeyboardEvent) {
                                            if (!mActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::TRANSLATE);
                                        }, "Scene Gizmo Translate");

    gInputManager->RegisterKeyCodeInput(SDLK_e,
                                        [this](KeyboardEvent) {
                                            if (!mActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::ROTATE);
                                        }, "Scene Gizmo Rotate");

    gInputManager->RegisterKeyCodeInput(SDLK_r,
                                        [this](KeyboardEvent) {
                                            if (!mActiveSceneCamera->IsCameraConsumingInput())
                                                ChangeImGuizmoOperation(ImGuizmo::SCALE);
                                        }, "Scene Gizmo Scale");
}


void Scene::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    mMouseX = static_cast<int>(aMouseMotion.x);
    mMouseY = static_cast<int>(aMouseMotion.y);
}

void Scene::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (mActiveSceneCamera->IsCameraConsumingInput())
        return;

    if (aMouseInput.button == SDL_BUTTON_LEFT
        && aMouseInput.state == SDL_PRESSED
        && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
        && !ImGuizmo::IsOver()) {
        const auto pickedRigidBody = PickRigidBody(mMouseX, mMouseY);
        for (const auto object: mObjects) {
            if (ColliderComponent comp; object->GetComponent<ColliderComponent>(comp)
                                        && pickedRigidBody == comp.GetRigidBody()) {
                mPickedEntity = object;
                break;
            }
        }
    }
}

void Scene::Construct() {
    for (const auto obj: mObjects) {
        obj->Construct();
    }
    assert(mActiveSceneCamera != nullptr);
}

void Scene::Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                   uint32_t aCurrentFrame) {
    for (const auto obj: mRenderSystems) {
        obj->mPipeline->Draw(aCommandBuffer, *this);
    }
}


void Scene::DrawObjectsRecursive(Entity *obj) {
    const char *nodeLabel = obj->GetUniqueLabel(obj->mName);

    if (IsParentOfPickedEntity(obj)) {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }

    ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (obj->mTransform.GetChildCount() == 0)
        nodeFlag = ImGuiTreeNodeFlags_Leaf;

    if (mPickedEntity == obj) {
        nodeFlag |= ImGuiTreeNodeFlags_Selected;
        ImGui::Begin("Picked Object");
        ImGui::Text(mPickedEntity->mName);
        obj->OnImGuiRender();
        ImGui::End();
    }

    if (ImGui::TreeNodeEx(nodeLabel, nodeFlag)) {
        for (auto childTransform: obj->mTransform.GetChildren()) {
            Entity *childEntity = mTransformEntityRelationships[childTransform];
            DrawObjectsRecursive(childEntity);
        }
        ImGui::TreePop();
    }

    if (ImGui::IsItemClicked()) {
        mPickedEntity = obj;
    }
}

bool Scene::IsParentOfPickedEntity(const Entity *obj) {
    if (mPickedEntity == nullptr) {
        return false;
    }
    for (const Entity *parentEntity = mPickedEntity; parentEntity != nullptr;
         parentEntity = mTransformEntityRelationships[parentEntity->mTransform.GetParent()]) {
        if (parentEntity == obj) {
            return true;
        }
    }
    return false;
}

void Scene::ChangeImGuizmoOperation(const int aOperation) {
    mCurrentGizmoOperation = static_cast<ImGuizmo::OPERATION>(aOperation);
}

void Scene::OnImGuiRender() {
    ImGui::Begin(mSceneName);

    const ImGuiIO &io = ImGui::GetIO();
    // Draw Gizmo Controls TODO: Add KB Control shortcuts
    ImGui::SeparatorText("Controls");
    ImGui::BeginChild(GetUniqueLabel("Controls"),
                      ImVec2(0.0f, 0.0f),
                      ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY); {
        if (ImGui::RadioButton(GetUniqueLabel("Translate"), mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Rotate"), mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if (ImGui::RadioButton(GetUniqueLabel("Scale"), mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
            if (ImGui::RadioButton(GetUniqueLabel("Local"), mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton(GetUniqueLabel("World"), mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
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
            mActiveSceneCamera->OnImGuiRender();
        }
        ImGui::Unindent();
    }

    if (mPickedEntity != nullptr) {
        // Draw Gizmos
        glm::mat4 matrix = mPickedEntity->mTransform.GetWorldMatrix();

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::AllowAxisFlip(false);
        if (ImGuizmo::Manipulate(glm::value_ptr(mActiveSceneCamera->GetViewMatrix()),
                                 glm::value_ptr(mActiveSceneCamera->GetPerspectiveMatrix()),
                                 mCurrentGizmoOperation, mCurrentGizmoMode,
                                 glm::value_ptr(matrix),NULL)) {
            mPickedEntity->mTransform.SetMatrix(matrix);
        }
    }

    if (ImGui::CollapsingHeader("Objects")) {
        for (const auto obj: mObjects) {
            ImGui::BeginGroup();
            ImGui::Indent(); {
                if (obj->mTransform.GetParent() == nullptr) {
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
                    for (const auto &renderSystem: mRenderSystems) {
                        vkDeviceWaitIdle(gGraphics->mLogicalDevice);
                        renderSystem->mPipeline->Destroy();
                        renderSystem->Create(renderSystem->GetBoundDescriptors());
                        renderSystem->mPipeline->Create();
                    }
                });
            }
            ImGui::Indent();
            for (auto system: mRenderSystems) {
                if (ImGui::CollapsingHeader(system->mPipeline->mPipelineName)) {
                    ImGui::Indent();
                    ImGui::Text("Material Count: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(system->mPipeline->mRenderers.size()).c_str());
                    for (const auto renderer: system->mPipeline->mRenderers) {
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
    mPhysicsSystem->Tick(aDeltaTime);
    mSceneInteractionPhysicsSystem->Tick(aDeltaTime);
    for (const auto obj: mObjects) {
        obj->Tick(aDeltaTime);
    }
}

void Scene::Cleanup() {
    Logger::Log(spdlog::level::info, (std::string("Cleaning up scene ") + mSceneName).c_str());
    for (const auto obj: mObjects) {
        obj->Cleanup();
        delete obj;
    }

    delete mActiveSceneCamera;
    for (const auto system: mRenderSystems) {
        system->mPipeline->Destroy();
        delete system->mPipeline.get();
    }
    mPhysicsSystem->Destroy();
    delete mPhysicsSystem;
    mPhysicsSystem = nullptr;

    mSceneInteractionPhysicsSystem->Destroy();
    delete mSceneInteractionPhysicsSystem;
    mSceneInteractionPhysicsSystem = nullptr;
}

void Scene::AddRenderSystem(RenderSystemBase *aRenderSystem) {
    mRenderSystems.push_back(aRenderSystem);
    aRenderSystem->mPipeline->Create();
}

// TODO: Make pointers managed
MeshObject *Scene::CreateObject(const char *aName, const char *aMeshPath, Material &aMaterial,
                                GraphicsPipeline &aPipeline, const glm::vec3 aPos, const glm::vec3 aRot,
                                const glm::vec3 aScale) {
    auto *object = new MeshObject();

    object->CreateObject(aMaterial, aName);
    object->mMeshRenderer.BindRenderer(aPipeline);
    object->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() + aMeshPath).c_str());

    object->mTransform.SetLocalPosition(aPos);
    object->mTransform.SetLocalRotation(aRot);
    object->mTransform.SetLocalScale(aScale);

    //auto *sceneCollider = new ColliderComponent();
    //sceneCollider->SetName("SceneCollider");
    //ColliderCreateInfo colliderInfo;
    //colliderInfo.collisionShape = CollisionHelper::MakeCollisionMesh(object->mMeshRenderer.mMesh->GetVertices(),
    //                                                                  object->mMeshRenderer.mMesh->GetIndices());
    ////   colliderInfo.collisionShape = CollisionHelper::MakeAABBCollision(object->mMeshRenderer.mMesh->GetVertices());
    //sceneCollider->Create(mSceneInteractionPhysicsSystem, colliderInfo);
    //object->AddComponent(sceneCollider);

    AddEntity(object);
    return object;
}

void Scene::AddEntity(Entity *aEntity) {
    mObjects.push_back(aEntity);
    mTransformEntityRelationships[&aEntity->mTransform] = aEntity;
}

void Scene::AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass,
                                 float aFriction) const {
    auto *sphereCollider = new ColliderComponent();
    ColliderCreateInfo sphereColliderInfo{};
    sphereColliderInfo.collisionShape = new btSphereShape(aRadius);
    sphereColliderInfo.mass = aMass;
    sphereColliderInfo.friction = aFriction;
    sphereCollider->Create(mPhysicsSystem, sphereColliderInfo);
    aEntity.AddComponent(sphereCollider);
}

void Scene::AttachBoxCollider(Entity &aEntity, const glm::vec3 aHalfExtents, const float aMass,
                              const float aFriction) const {
    auto *boxCollider = new ColliderComponent();
    ColliderCreateInfo boxColliderInfo{};
    boxColliderInfo.collisionShape = new btBoxShape(btVector3(aHalfExtents.x, aHalfExtents.y, aHalfExtents.z));
    boxColliderInfo.mass = aMass;
    boxColliderInfo.friction = aFriction;
    boxCollider->Create(mPhysicsSystem, boxColliderInfo);
    aEntity.AddComponent(boxCollider);
}

const btRigidBody *Scene::PickRigidBody(const int x, const int y) const {
    const Ray ray = mActiveSceneCamera->GetRayTo(x, y);
    const btVector3 rayFrom(CollisionHelper::GlmToBullet(ray.origin));
    const btVector3 rayTo(CollisionHelper::GlmToBullet(ray.origin + ray.direction * mActiveSceneCamera->mZFar));

    btCollisionWorld::ClosestRayResultCallback RayCallback(rayFrom, rayTo);

    mPhysicsSystem->mDynamicsWorld->rayTest(rayFrom, rayTo, RayCallback);
    if (RayCallback.hasHit()) {
        if (const btRigidBody *pickedBody = btRigidBody::upcast(RayCallback.m_collisionObject)) {
            return pickedBody;
        }
    }

    return nullptr;
}

Texture *Scene::CreateTexture(const char *aName, std::vector<std::string> aPathsSet) {
    if (mLoadedTextures.find(aName) == mLoadedTextures.end()) {
        mLoadedTextures[aName] = std::make_unique<Texture>();

        auto *texture = mLoadedTextures[aName].get();
        texture->LoadImagesFromDisk(aPathsSet);
        texture->Create();

        return texture;
    }

    Logger::Log(spdlog::level::err, "Texture Already Exists");
    return nullptr;
}
