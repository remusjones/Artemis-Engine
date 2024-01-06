#include "DefaultMaterial.h"

#include "imgui.h"
#include "LoadUtilities.h"
#include "VulkanGraphicsImpl.h"
#include "File Management/FileManagement.h"
#include "Vulkan/VulkanEngine.h"

void DefaultMaterial::Create(MaterialBase *aBaseMaterial, const char *aMaterialName) {

    AddBinding(SCENE_INFORMATION, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
    AddBinding(PROPERTIES, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);
    AddBinding(TEXTURE, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    Material::Create(this, "Default Material");

    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mSceneBuffer, SCENE_INFORMATION, 0);

    CreateProperties(PROPERTIES, MaterialProperties());

    MakeDefaults();
}

void DefaultMaterial::OnImGuiRender() {
    Material::OnImGuiRender();
    ImGui::SeparatorText("Material");
    ImGui::ColorEdit4(GetUniqueLabel("Color"), &mMaterialProperties.mColor[0]);
    if (ImGui::DragFloat(GetUniqueLabel("Shininess"),
                         &mMaterialProperties.mShininess, 0.1f)) {
    }
    if (ImGui::DragFloat(GetUniqueLabel("Specular"),
                         &mMaterialProperties.mSpecularStrength, 0.1f)) {
    }
    int tmp = mMaterialProperties.mDebugRenderState;
    if (ImGui::Combo(GetUniqueLabel("Debug"), &tmp, mDebugColors.data(), mDebugColors.size())) {
        mMaterialProperties.mDebugRenderState = tmp;
    }
}
static std::unique_ptr<Texture> mDefaultTexture = nullptr;
void DefaultMaterial::MakeDefaults() {
    if (mDefaultTexture == nullptr) {
        mDefaultTexture = std::make_unique<Texture>();
        std::vector<Color_RGBA> defaultColors;
        std::vector<std::string> paths;
        paths.push_back(FileManagement::GetWorkingDirectory() + DefaultAssetPaths[ALBEDO]);
        paths.push_back(FileManagement::GetWorkingDirectory() + DefaultAssetPaths[NORMAL]);
        mDefaultTexture->LoadImagesFromDisk(paths);
        mDefaultTexture->Create();
    }
    BindTexture(*mDefaultTexture, TEXTURE);
}

void DefaultMaterial::Destroy() {
    if (mDefaultTexture != nullptr) {
        mDefaultTexture->Destroy();
        mDefaultTexture.reset();
    }

    Material::Destroy();
}
