//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Color.h"
#include "Base/Common/Material.h"
#include "Components/Collision/ColliderComponent.h"
#include "../../IO/FileManagement.h"
#include "Objects/Camera.h"
#include "Objects/FlyCamera.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Common/Primative.h"
#include "Vulkan/Renderers/SkyboxRenderer.h"
#include "Vulkan/Materials/Cubemap.h"
#include "Vulkan/Materials/DefaultMaterial.h"
#include "Vulkan/Renderers/LineRenderer.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SandboxScene::PreConstruct(const char *aSceneName) {
    Scene::PreConstruct(aSceneName);
}

void SandboxScene::Construct() {

    // Todo move rendering setup to a separate function
    //
    // Create Render Pipelines
    //
    GraphicsPipeline::DefaultPipelineConfigInfo(mDefaultPipelineConfig);

    // todo: do I need to actually allocate these on the heap?
    mPBRPipeline = std::make_unique<PBRRenderSystem>();
    mUnlitPipeline = std::make_unique<UnlitRenderSystem>();
    mCubemapPipeline = std::make_unique<SkyboxRenderSystem>();
    mWireframeRenderSystem = std::make_unique<WireframeRenderSystem>();
    mLineRenderPipeline = std::make_unique<LineRenderSystem>();

    //
    // Define Material Usages
    //
    // todo: do I need to actually allocate these on the heap?
    Material* monkeyTexturedMaterial = mMaterialUnlitFactory.Create<DefaultMaterial>("Monkey Unlit");
    Material* unlitMaterial = mMaterialUnlitFactory.Create<DefaultMaterial>("Unlit");
    Material* teapotMaterial = mMaterialPBRFactory.Create<DefaultMaterial>("Teapot PBR");
    Material* lightMaterial = mMaterialUnlitFactory.Create<DefaultMaterial>("Light Unlit");
    Material* sphereMaterial = mMaterialPBRFactory.Create<DefaultMaterial>("Sphere PBR");
    Material* cubeMaterial = mMaterialPBRFactory.Create<DefaultMaterial>("Cube PBR");
    mCubemap = mGenericMaterialFactory.Create<Cubemap>("Skybox Cubemap");

    //
    // Make Materials
    //
    mMaterialUnlitFactory.MakeMaterials();
    mMaterialPBRFactory.MakeMaterials();
    mGenericMaterialFactory.MakeMaterials();

    //
    // Bind Materials
    //
    mUnlitPipeline->Create(mMaterialUnlitFactory.GetDescriptorLayouts());
    mLineRenderPipeline->Create(mMaterialUnlitFactory.GetDescriptorLayouts());
    mPBRPipeline->Create(mMaterialPBRFactory.GetDescriptorLayouts());
    mWireframeRenderSystem->Create(mMaterialUnlitFactory.GetDescriptorLayouts());

    std::vector<VkDescriptorSetLayout> mCubemapLayouts;
    mCubemapLayouts.push_back(mCubemap->GetDescriptorLayout());
    mCubemapPipeline->Create(mCubemapLayouts);

    //
    // Create Objects
    // TODO: Create constructor helper to make this smaller?
    //
    mMonkey = CreateObject("Monkey",
                         "/Assets/Models/monkey_smooth.obj", *monkeyTexturedMaterial,
                         *mWireframeRenderSystem->m_graphicsPipeline,
                         glm::vec3(2, 0, -5),
                         glm::vec3(0),
                         glm::vec3(1.f));


    mTeapot = CreateObject("Teapot",
                         "/Assets/Models/teapot.obj", *teapotMaterial,
                         *mPBRPipeline->m_graphicsPipeline,
                         glm::vec3(2, 0, -20),
                         glm::vec3(0),
                         glm::vec3(0.1f));

    mLight = CreateObject("Light",
                        "/Assets/Models/sphere.obj", *lightMaterial,
                        *mUnlitPipeline->m_graphicsPipeline,
                        glm::vec3(0, 0, 0),
                        glm::vec3(0),
                        glm::vec3(0.2));

    mMonkey->m_transform.SetParent(&mLight->m_transform);

    constexpr int uniformSphereCount = 3;
    for (int i = 0; i < uniformSphereCount; i++) {
        for (int j = 0; j < uniformSphereCount; j++) {
            for (int k = 0; k < uniformSphereCount; k++) {
                constexpr float sphereRadius = 0.5f;
                MeshObject *sphere = CreateObject("Physics Sphere",
                                                "/Assets/Models/sphere.obj", *sphereMaterial,
                                                *mPBRPipeline->m_graphicsPipeline,
                                                glm::vec3(i, j, k),
                                                glm::vec3(0),
                                                glm::vec3(sphereRadius));

                AttachSphereCollider(*sphere, sphereRadius, 0.25f, 0.5);
            }
        }
    }


    constexpr glm::vec3 floorScale(50, 0.5f, 50);
    mFloor = CreateObject("Floor", "/Assets/Models/cube.obj",
                        *cubeMaterial, *mPBRPipeline->m_graphicsPipeline,
                        glm::vec3(0, -10, 0), glm::vec3(0), floorScale);

    AttachBoxCollider(*mFloor, floorScale, 0);

    const std::vector stoneSet{
        FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall.png",
        FileManagement::GetWorkingDirectory() + "/Assets/Textures/Stone/Stone Wall_NRM.png"
    };
    const auto stoneTexture = CreateTexture("stoneTexture", stoneSet);
    teapotMaterial->BindTexture(stoneTexture->mImageBufferInfo, DefaultMaterial::TEXTURE);

    //
    // Skybox TODO: Skybox Constructor Required
    //
    mCubemapMesh = new Primative("Skybox");
    auto *mSkyboxRenderer = new SkyboxRenderer();
    mCubemapMesh->mRenderer = mSkyboxRenderer;
    mSkyboxRenderer->mMaterial = mCubemap;
    mSkyboxRenderer->mTransform = &mCubemapMesh->m_transform;
    mSkyboxRenderer->BindRenderer(*mCubemapPipeline->m_graphicsPipeline);
    mSkyboxRenderer->LoadMesh((FileManagement::GetWorkingDirectory() +
                               std::string("/Assets/Models/cube.obj")).c_str());

    //
    // Scene Camera
    //
    m_ActiveSceneCamera = new FlyCamera();
    m_ActiveSceneCamera->Construct();
    m_ActiveSceneCamera->m_transform.SetLocalPosition({0, 0, -5.0f});

    mLineRendererEntity = new Primative("LineRenderer");
    mLineRendererEntity->mRenderer = mLineRenderer;

    mLineRenderer = new LineRenderer();
    mLineRenderer->mTransform = &mLineRendererEntity->m_transform;
    mLineRenderer->SetLinePositions(
        {
            glm::vec3(0, -10, 0),
            glm::vec3(0, 10, 0),
            glm::vec3(10, 10, 0),
            glm::vec3(10, -10, 0)
        });

    mLineRenderer->mMaterial = unlitMaterial;
    mLineRenderer->BindRenderer(*mLineRenderPipeline->m_graphicsPipeline);

    //
    // Setup Scene Dependencies
    // Register to scene TODO: Review if we auto-register these
    //
    AddEntity(mCubemapMesh);
    AddEntity(mLineRendererEntity);

    //
    // Setup Draw Order
    //
    AddRenderSystem(mCubemapPipeline.get());
    AddRenderSystem(mUnlitPipeline.get());
    AddRenderSystem(mPBRPipeline.get());
    AddRenderSystem(mWireframeRenderSystem.get());
    AddRenderSystem(mLineRenderPipeline.get());
    //
    // Construct Scene
    //
    Scene::Construct();
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    mLight->mMeshRenderer.mMaterial->mMaterialProperties.mColor =
            glm::vec4(mSceneData.color.x, mSceneData.color.y, mSceneData.color.z, 1);

    mSceneData.position = mLight->m_transform.GetLocalPosition();
    mSceneData.mViewMatrix = m_ActiveSceneCamera->GetViewMatrix();
    mSceneData.mViewPos = glm::vec4(m_ActiveSceneCamera->m_transform.GetLocalPosition(), 1.0f);
    mSceneData.mViewProjectionMatrix = m_ActiveSceneCamera->GetPerspectiveMatrix();

    mMonkey->m_transform.RotateAxisLocal(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    Scene::Tick(aDeltaTime);


    mLineRenderer->SetLinePositions({
                                        mTeapot->m_transform.GetWorldPosition(),
                                        mMonkey->m_transform.GetWorldPosition(),
                                        mLight->m_transform.GetWorldPosition(),
                                        mFloor->m_transform.GetWorldPosition()
                                    }, {
                                        Color::Red(),
                                        Color::Green(),
                                        Color::Blue(),
                                        Color::Yellow()
                                    });

    mLineRenderer->DrawLine(
        mFloor->m_transform.GetWorldPosition(),
        mMonkey->m_transform.GetWorldPosition(),
        Color::Magenta());

    m_ActiveSceneCamera->Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    for (const auto &loadedTextures: mLoadedTextures) {
        loadedTextures.second->Destroy();
    }
    mLineRenderer->DestroyRenderer();
    mMaterialUnlitFactory.DestroyMaterials();
    mMaterialPBRFactory.DestroyMaterials();
    mGenericMaterialFactory.DestroyMaterials();
    Scene::Cleanup();
}

void SandboxScene::OnImGuiRender() {
    Scene::OnImGuiRender();
}
