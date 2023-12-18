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
    std::vector<const MaterialBase *> MakeMaterials(uint8_t aBinding) const;
    void UpdateDescriptorSets(uint32_t aDescriptorIndex, VkBuffer aBuffer);

    void AddRenderer(Renderer* aRenderer);
    void RenderPipeline(VkCommandBuffer aCommandBuffer, uint32_t aImageIndex, uint32_t aCurrentFrame);
    void Create();
    void Destroy() const;



    VkDescriptorSetLayout mDescriptorSetLayout;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<const MaterialBase*> mMaterials = {};
    std::vector<Renderer*> mRenderers = {};
    std::vector<VkDescriptorSet> mDescriptorSets;
private:
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateUniformBufferLayouts();

private:
    VkDescriptorPool mDescriptorPool;

};
