//
// Created by Remus on 22/12/2023.
//

#pragma once
#include <vector>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Components/Collision/Ray.h"
#include "Objects/FlyCamera.h"
#include "Objects/ImGuiLayer.h"

class btRigidBody;
class btVector3;
class Material;
class PhysicsSystem;
class GraphicsPipeline;
class Camera;
class MeshObject;

class Scene : public ImGuiLayer {
public:
    virtual ~Scene() = default;

    virtual void PreConstruct(const char *aSceneName);

    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);

    virtual void Construct();

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame);

    void OnImGuiRender() override;

    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    void AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline);
    MeshObject *MakeObject(const char* aName,
                           const char* aMeshPath,
                           Material &aMaterial,
                           GraphicsPipeline &aPipeline,
                           glm::vec3 aPos = glm::vec3(0),
                           glm::vec3 aRot = glm::vec3(0),
                           glm::vec3 aScale = glm::vec3(1)
    );

    void AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass, float aFriction = 0.5f) const;
    void AttachBoxCollider(Entity &aEntity, glm::vec3 aHalfExtents, float aMass, float aFriction = 0.5f) const;

    const btRigidBody *PickRigidBody(int x, int y);

    btRigidBody* PickBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
    Ray GetRayTo(int x, int y) const;





    FlyCamera *mActiveSceneCamera;
    // TODO: cleanup these in scene, instead of engine
    std::vector<GraphicsPipeline *> mGraphicsPipelines;
    GPUSceneData mSceneData;
    const char *mSceneName; //
    std::vector<Entity *> mObjects;
    PhysicsSystem *mPhysicsSystem;


    PhysicsSystem *mSceneInteractionPhysicsSystem;
    int mMouseX, mMouseY;
};
