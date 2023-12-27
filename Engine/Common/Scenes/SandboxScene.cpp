//
// Created by Remus on 22/12/2023.
//

#include "SandboxScene.h"

#include "LoadUtilities.h"
#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"
#include "File Management/FileManagement.h"
#include "glog/logging.h"
#include "Objects/Camera.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Common/MeshObject.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

void SandboxScene::Construct(const char *aSceneName) {
    // Create basic mesh pipeline
    GraphicsPipeline *litMeshPipeline = new GraphicsPipeline("Lit Mesh Pipeline");
    GraphicsPipeline *unlitMeshPipeline = new GraphicsPipeline("Unlit Mesh Pipeline");
    GraphicsPipeline *texturedMeshPipeline = new GraphicsPipeline("Textured Mesh Pipeline");

    litMeshPipeline->AddShader("/Shaders/3DObject_v.spv",
                               VK_SHADER_STAGE_VERTEX_BIT);
    litMeshPipeline->AddShader("/Shaders/Lit_f.spv",
                               VK_SHADER_STAGE_FRAGMENT_BIT);


    unlitMeshPipeline->AddShader("/Shaders/3DUnlit_v.spv",
                                 VK_SHADER_STAGE_VERTEX_BIT);
    unlitMeshPipeline->AddShader("/Shaders/Lit_f.spv",
                                 VK_SHADER_STAGE_FRAGMENT_BIT);

    texturedMeshPipeline->AddShader("/Shaders/3DObject_v.spv",
                                    VK_SHADER_STAGE_VERTEX_BIT);
    texturedMeshPipeline->AddShader("/Shaders/TexturedLit_f.spv",
                                    VK_SHADER_STAGE_FRAGMENT_BIT);


    mMonkey = new MeshObject();
    mMonkey->CreateObject(*litMeshPipeline, "Monkey");
    mMonkey->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/monkey_smooth.obj")).c_str());


    mTeapot = new MeshObject();
    mTeapot->CreateObject(*texturedMeshPipeline, "Teapot");
    mTeapot->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/teapot.obj")).c_str());


    mLight = new MeshObject();
    mLight->CreateObject(*unlitMeshPipeline, "Light");
    mLight->LoadMesh(
        (FileManagement::GetWorkingDirectory() +
         std::string("/../Models/sphere.obj")).c_str());


    mMonkey->mTransform.SetPosition({-2, 0, 0});
    mTeapot->mTransform.SetPosition({2, 0, 0});
    mLight->mTransform.SetScale({0.1f, 0.1f, 0.1f});

    mObjects.push_back(mMonkey);
    mObjects.push_back(mTeapot);
    mObjects.push_back(mLight);


    // TODO: Condense into texture create function
    Texture texture;
    LoadUtilities::LoadImageFromDisk(gGraphics, "../Textures/textureTest.png", texture.image);
    VkImageViewCreateInfo imageinfo = VulkanInitialization::ImageViewCreateInfo(
        VK_FORMAT_R8G8B8A8_SRGB, texture.image.mImage,
        VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(gGraphics->mLogicalDevice, &imageinfo, nullptr, &texture.imageView);
    mLoadedTextures["textureTest"] = texture;


    mActiveCamera = new Camera();
    mActiveCamera->mTransform.SetPosition({0, 0, -5.0f});


    AddGraphicsPipeline(litMeshPipeline);
    AddGraphicsPipeline(unlitMeshPipeline);
    AddGraphicsPipeline(texturedMeshPipeline);


    // TODO Move to LoadTexture(...) call on material?
    VkSamplerCreateInfo samplerInfo = VulkanInitialization::SamplerCreateInfo(VK_FILTER_NEAREST);
    VkSampler blockySampler;
    vkCreateSampler(gGraphics->mLogicalDevice, &samplerInfo, nullptr, &blockySampler);

    VkDescriptorImageInfo imageBufferInfo;
    imageBufferInfo.sampler = blockySampler;
    imageBufferInfo.imageView = mLoadedTextures["textureTest"].imageView;
    imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet texture1 = VulkanInitialization::WriteDescriptorImage(
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, mTeapot->mMaterial->GetDescriptorSet(), &imageBufferInfo, 3);

    vkUpdateDescriptorSets(gGraphics->mLogicalDevice, 1, &texture1, 0, nullptr);

    Scene::Construct(aSceneName);
}

float deltaAccumulated;

void SandboxScene::Tick(float aDeltaTime) {
    deltaAccumulated += aDeltaTime / 5;
    float yPosition = 1.0f * sin(2 * M_PI * 1 * deltaAccumulated);

    mLight->mTransform.SetPosition({0, yPosition, 0.0f});
    mLight->mMaterial->mMaterialProperties.mColor =
            glm::vec4(mSceneData.color.x, mSceneData.color.y, mSceneData.color.z, 1);

    mSceneData.position = mLight->mTransform.Position();

    mMonkey->mTransform.RotateAround(aDeltaTime / 5, glm::vec3(0.0f, 1, 0));
    mTeapot->mTransform.RotateAround(aDeltaTime / 10, glm::vec3(0.0f, 1.0f, 1.0f));


    Scene::Tick(aDeltaTime);
}

void SandboxScene::Cleanup() {
    for (const auto loadedTextures: mLoadedTextures) {
        vmaDestroyImage(gGraphics->mAllocator,
                        loadedTextures.second.image.mImage,
                        loadedTextures.second.image.mAllocation);
        vkDestroyImageView(gGraphics->mLogicalDevice, loadedTextures.second.imageView, nullptr);
    }
    Scene::Cleanup();
}
