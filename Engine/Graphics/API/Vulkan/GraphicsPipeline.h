//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <vector>
#include "Objects/Camera.h"
class Scene;
class Material;
class MaterialBase;
class Renderer;
class GraphicsPipeline {
public:
    GraphicsPipeline(const char* aPipelineName = "Unknown") : mPipelineName(aPipelineName){};

    void AddShader(const char* aPath, VkShaderStageFlagBits aStage);
    void AddMaterialBase(const MaterialBase* aBase) {
        mMaterials.push_back(aBase);
    };
    std::vector<const MaterialBase *> MakeMaterials(uint8_t aBinding) const;
    void UpdateDescriptorSets(uint32_t aDescriptorIndex, VkBuffer aBuffer);

    void AddRenderer(Renderer* aRenderer);
    void Draw(VkCommandBuffer aCommandBuffer, Scene &aScene) const;

    void Create();
    void Destroy() const;
    void BindDescriptor();
    void BindPushConstant(VkPushConstantRange aPushConstant);

private:
    void CreateUniformBufferLayouts();

public:
    const char* mPipelineName;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<const MaterialBase*> mMaterials = {};
    std::vector<Renderer*> mRenderers = {};
    VkPipelineDepthStencilStateCreateInfo mDepthStencil;
};
