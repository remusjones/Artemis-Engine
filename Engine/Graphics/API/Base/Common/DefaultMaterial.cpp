#include "DefaultMaterial.h"

#include "LoadUtilities.h"
#include "VulkanGraphicsImpl.h"
#include "Vulkan/VulkanEngine.h"

void DefaultMaterial::Create(MaterialBase *aBaseMaterial, const char *aMaterialName) {
    AddBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);

    // Binds Scene Lighting Uniform Buffer
    AddBinding(1, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);


    // Material Properties
    AddBinding(2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);

    // Texture Binding
    // TODO: Move into set
    AddBinding(3, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    AddBinding(4, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    Material::Create(this, "Default Material");

    //for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
    //    SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mCameraBuffer, 0, 0);

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mSceneBuffer, 0, 0);

    CreateProperties(1, MaterialProperties());

    MakeDefaults();
}

void DefaultMaterial::MakeDefaults() {
    mDefaultAlbedo = std::make_unique<Texture>();
    mDefaultAlbedo->CreateDefault(Color_RGBA(255,255,255,255));
    mDefaultNormal = std::make_unique<Texture>();
    mDefaultNormal->CreateDefault(Color_RGBA(128, 128, 255, 255));

    BindTexture(*mDefaultAlbedo, 3);
    BindTexture(*mDefaultNormal, 4);
}

void DefaultMaterial::Destroy() {
    mDefaultAlbedo->Destroy();
    mDefaultNormal->Destroy();
    Material::Destroy();
}
