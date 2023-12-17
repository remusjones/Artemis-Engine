//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vector>
class Material;
class MaterialBase;
class Renderer;
class GraphicsPipeline {
public:
    void AddShader(const char* aPath, VkShaderStageFlagBits aStage);
    void AddMaterialBase(const MaterialBase* aBase) {
        mMaterials.push_back(aBase);
    };
    std::vector<Material> MakeMaterials(uint8_t aBinding) const;

    void AddRenderer(Renderer* aRenderer);
    void RenderPipeline(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex);
    void Create();
    void Destroy() const;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<const MaterialBase*> mMaterials = {};
    std::vector<Renderer*> mRenderers = {};
};
