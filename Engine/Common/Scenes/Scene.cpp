//
// Created by Remus on 22/12/2023.
//

#include "Scene.h"
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "ImGuizmo.h"
#include "Logger.h"

#include "VulkanGraphicsImpl.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Components/Collision/ColliderComponent.h"
#include "Components/Collision/CollisionHelper.h"
#include "File Management/FileManagement.h"
#include "Physics/PhysicsSystem.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Systems/GraphicsPipeline.h"
#include "Components/Component.h"


void Scene::PreConstruct(const char *aSceneName) {
    mSceneName = aSceneName;

    mPhysicsSystem = new PhysicsSystem();
    mSceneInteractionPhysicsSystem = new PhysicsSystem();

    mPhysicsSystem->Create();
    mSceneInteractionPhysicsSystem->Create();
    gInputManager->RegisterMouseInput([&](SDL_MouseMotionEvent motion) { MouseMovement(motion); },
                                      "Scene Mouse Movement");
    gInputManager->RegisterMouseInput([&](SDL_MouseButtonEvent input) { MouseInput(input); }, "Scene Mouse Press");
}

void Scene::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    mMouseX = aMouseMotion.x;
    mMouseY = aMouseMotion.y;
}

void Scene::MouseInput(const SDL_MouseButtonEvent &aMouseInput) {
    if (aMouseInput.button == SDL_BUTTON_LEFT) {
        auto t = PickRigidBody(mMouseX, mMouseY);
        for (auto object: mObjects) {
            if (ColliderComponent comp; object->GetComponent<ColliderComponent>("SceneCollider", comp)
                && t == comp.GetRigidBody()) {
                Logger::Log(spdlog::level::info, object->mName);
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
    for (auto obj: mGraphicsPipelines) {
        obj->Draw(aCommandBuffer, *this);
    }

    OnImGuiRender();
}

// Declaring these here due to circular deps
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

void Scene::OnImGuiRender() {
    ImGui::Begin(mSceneName);

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

    // Draw Objects in scene
    if (ImGui::CollapsingHeader("Objects")) {
        for (const auto obj: mObjects) {
            ImGui::BeginGroup();
            ImGui::Indent(); {
                if (ImGui::CollapsingHeader(obj->GetUniqueLabel(obj->mName))) {
                    ImGui::Indent();

                    // Draw Gizmos
                    glm::mat4 matrix = obj->mTransform.GetWorldMatrix();
                    const ImGuiIO &io = ImGui::GetIO();

                    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                    if (ImGuizmo::Manipulate(glm::value_ptr(mActiveSceneCamera->GetViewMatrix()),
                                             glm::value_ptr(mActiveSceneCamera->GetPerspectiveMatrix()),
                                             mCurrentGizmoOperation, mCurrentGizmoMode,
                                             glm::value_ptr(matrix),NULL)) {
                        obj->mTransform.SetMatrix(matrix);
                    }
                    // Draw Object UI
                    obj->OnImGuiRender();

                    ImGui::Unindent();
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
        ImGui::Text(std::to_string(static_cast<int32_t>(mGraphicsPipelines.size())).c_str());
        if (ImGui::CollapsingHeader("Graphic Systems Info")) {
            ImGui::Indent();
            for (const auto pipeline: mGraphicsPipelines) {
                if (ImGui::CollapsingHeader(pipeline->mPipelineName)) {
                    ImGui::Indent();
                    ImGui::Text("Material Count: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(pipeline->mRenderers.size()).c_str());
                    for (const auto renderer: pipeline->mRenderers) {
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
    for (const auto pipeline: mGraphicsPipelines) {
        pipeline->Destroy();
        delete pipeline;
    }
    mPhysicsSystem->Destroy();
    delete mPhysicsSystem;
    mPhysicsSystem = nullptr;

    mSceneInteractionPhysicsSystem->Destroy();
    delete mSceneInteractionPhysicsSystem;
    mSceneInteractionPhysicsSystem = nullptr;
}

void Scene::AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline) {
    mGraphicsPipelines.push_back(aGraphicsPipeline);
    aGraphicsPipeline->Create();
}

MeshObject *Scene::MakeObject(const char *aName, const char *aMeshPath, Material &aMaterial,
                              GraphicsPipeline &aPipeline, const glm::vec3 aPos, const glm::vec3 aRot,
                              const glm::vec3 aScale) {
    auto *object = new MeshObject();

    object->CreateObject(aMaterial, aName);
    object->mMeshRenderer.BindRenderer(aPipeline);
    object->mMeshRenderer.LoadMesh((FileManagement::GetWorkingDirectory() + aMeshPath).c_str());

    object->mTransform.SetLocalPosition(aPos);
    object->mTransform.SetLocalRotation(aRot);
    object->mTransform.SetLocalScale(aScale);

    auto *sceneCollider = new ColliderComponent();
    sceneCollider->SetName("SceneCollider");
    ColliderCreateInfo colliderInfo;
    colliderInfo.collisionShape = CollisionHelper::MakeCollisionMesh(object->mMeshRenderer.mMesh->GetVertices(),
                                                                     object->mMeshRenderer.mMesh->GetIndices());
    sceneCollider->Create(mSceneInteractionPhysicsSystem, colliderInfo);
    object->AddComponent(sceneCollider);
    mObjects.push_back(object);

    return object;
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

void Scene::AttachBoxCollider(Entity &aEntity, glm::vec3 aHalfExtents, float aMass, float aFriction) const {
    auto *boxCollider = new ColliderComponent();
    ColliderCreateInfo boxColliderInfo{};
    boxColliderInfo.collisionShape = new btBoxShape(btVector3(aHalfExtents.x, aHalfExtents.y, aHalfExtents.z));
    boxColliderInfo.mass = aMass;
    boxColliderInfo.friction = aFriction;
    boxCollider->Create(mPhysicsSystem, boxColliderInfo);
    aEntity.AddComponent(boxCollider);
}

const btRigidBody *Scene::PickRigidBody(int x, int y)
{
    glm::vec3 rayToWorld = GetRayTo(x, y);

    btVector3 rayFrom(CollisionHelper::GlmToBullet(mActiveSceneCamera->mTransform.GetWorldPosition()));
    btVector3 rayTo(rayToWorld.x, rayToWorld.y, rayToWorld.z);

    btCollisionWorld::ClosestRayResultCallback RayCallback(rayFrom, rayTo);

    mSceneInteractionPhysicsSystem->mDynamicsWorld->rayTest(rayFrom, rayTo, RayCallback);
    if (RayCallback.hasHit())
    {
        const btRigidBody *pickedBody = btRigidBody::upcast(RayCallback.m_collisionObject);
        if (pickedBody)
        {
            return pickedBody;
        }
    }

    return nullptr;
}

glm::vec3 Scene::GetRayTo(const int x, const int y) const
{
    const float zDepth = 10000.f;
    float normalizedPointX = (2.0f * x) / gGraphics->mSwapChain->mSwapChainExtent.width - 1.0f;
    float normalizedPointY = 1.0f - (2.0f * y) / gGraphics->mSwapChain->mSwapChainExtent.height;

    glm::vec4 rayClip = glm::vec4(normalizedPointX, normalizedPointY, -1.0, 1.0);
    glm::vec4 rayEye = glm::inverse(mActiveSceneCamera->GetPerspectiveMatrix()) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    glm::vec3 rayWorld = glm::vec3(inverse(mActiveSceneCamera->GetViewMatrix()) * rayEye);
    rayWorld = glm::normalize(rayWorld);

    return mActiveSceneCamera->mTransform.GetWorldPosition() + rayWorld * zDepth;
}
