//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vector>
class Material;
class MaterialBase;
class GraphicsPipeline {
public:
    void AddShader(const char* aPath, VkShaderStageFlagBits aStage);
    void AddMaterialBase(const MaterialBase* aBase) {
        mMaterials.push_back(aBase);
    };
    std::vector<Material> MakeMaterials(uint8_t aBinding) const;
    void Create();
    void Destroy() const;

    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<const MaterialBase*> mMaterials = {};
};
