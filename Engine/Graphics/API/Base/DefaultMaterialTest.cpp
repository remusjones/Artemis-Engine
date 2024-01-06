#include "DefaultMaterialTest.h"

#include "imgui.h"
#include "VulkanGraphicsImpl.h"
#include "Vulkan/VulkanEngine.h"

void DefaultMaterialTest::Create(MaterialBase *aBaseMaterial, const char *aMaterialName) {
    AddBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);

    // Binds Scene Lighting Uniform Buffer
    AddBinding(1, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);


    // Material Properties
    AddBinding(2, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL);

    AddBinding(TEXTURE, 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

    Material::Create(this, "Default2 Material");


    for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; i++)
        SetBuffers(gGraphics->mVulkanEngine.GetFrame(i).mSceneBuffer, 0, 0);

    CreateProperties(1, MaterialProperties());

    MakeDefaults();
}

void DefaultMaterialTest::OnImGuiRender() {
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

void DefaultMaterialTest::MakeDefaults() {
    // TODO: impl default array creation
}

void DefaultMaterialTest::Destroy() {
    Material::Destroy();
}
