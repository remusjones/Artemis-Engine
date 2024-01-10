//
// Created by Remus on 17/12/2023.
//

#pragma once
#include <vector>

#include "PipelineConfigInfo.h"
#include "Base/Common/Material.h"

struct PipelineConfigInfo;
class Scene;
class Material;
class MaterialBase;
class Renderer;

class GraphicsPipeline {
public:
    enum class SubPasses {
        SUBPASS_GEOMETRY = 0,
        SUBPASS_LIGHTING,
        SUBPASS_TRANSPARENCY,
        NUMBER_OF_SUBPASSES
    };

    GraphicsPipeline(const char *aPipelineName,
        const PipelineConfigInfo &aConfigInfo) : mPipelineName(
        aPipelineName) {
        mPipelineConfig = aConfigInfo;
    }

    void CreateShaderModule(const char *aPath, VkShaderStageFlagBits aStage);
    void AddRenderer(Renderer *aRenderer);
    void Draw(VkCommandBuffer aCommandBuffer, const Scene &aScene) const;
    void Create();
    void Destroy();
    static void DefaultPipelineConfigInfo(PipelineConfigInfo &aConfigInfo);

    const char *mPipelineName;
    PipelineConfigInfo mPipelineConfig;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<Renderer *> mRenderers = {};
    VkPipelineDepthStencilStateCreateInfo mDepthStencil;
};
