//
// Created by Remus on 22/12/2023.
//

#pragma once

#include <memory>

#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "Base/Common/Data/GPUSceneData.h"
#include "Objects/FlyCamera.h"
#include "Objects/ImGuiLayer.h"

class Texture;
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


    virtual void Construct();

    virtual void Render(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex,
                        uint32_t aCurrentFrame);


    virtual void Tick(float aDeltaTime);

    virtual void Cleanup();

    void OnImGuiRender() override;

    void AddGraphicsPipeline(GraphicsPipeline *aGraphicsPipeline);


    MeshObject *CreateObject(const char *aName,
                           const char *aMeshPath,
                           Material &aMaterial,
                           GraphicsPipeline &aPipeline,
                           glm::vec3 aPos = glm::vec3(0),
                           glm::vec3 aRot = glm::vec3(0),
                           glm::vec3 aScale = glm::vec3(1)
    );

    void AddEntity(Entity *aEntity);

    void AttachSphereCollider(Entity &aEntity, const float aRadius, const float aMass, float aFriction = 0.5f) const;

    void AttachBoxCollider(Entity &aEntity, glm::vec3 aHalfExtents, float aMass, float aFriction = 0.5f) const;

    // TODO: probably bind these to flycam instead?
    void MouseMovement(const SDL_MouseMotionEvent &aMouseMotion);

    void MouseInput(const SDL_MouseButtonEvent &aMouseInput);

    const btRigidBody *PickRigidBody(int x, int y) const;

    Texture* CreateTexture(const char *aName, std::vector<std::string> aPathsSet);
private:
    void DrawObjectsRecursive(Entity *obj);
    bool IsParentOfPickedEntity(const Entity *obj);

public:
    FlyCamera *mActiveSceneCamera;
    std::vector<GraphicsPipeline *> mGraphicsPipelines;
    GPUSceneData mSceneData;
    const char *mSceneName; //
    PhysicsSystem *mPhysicsSystem;

protected:
    std::vector<Entity *> mObjects;
    std::unordered_map<Transform *, Entity *> mTransformEntityRelationships;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mLoadedTextures;
private:
    PhysicsSystem *mSceneInteractionPhysicsSystem;
    Entity *mPickedEntity{nullptr};
    int mMouseX{0}, mMouseY{0};
};
