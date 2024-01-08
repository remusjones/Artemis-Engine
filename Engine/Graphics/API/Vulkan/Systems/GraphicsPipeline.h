//
// Created by Remus on 17/12/2023.
//

#pragma once


#include <memory>
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

    void AddMaterialBase(const std::shared_ptr<Material> aBase) {
        mMaterials.push_back(aBase);
    }

    void AddRenderer(Renderer *aRenderer);

    void Draw(VkCommandBuffer aCommandBuffer, const Scene &aScene) const;

    void Create();

    void Destroy() const;

    template<typename T>
    std::shared_ptr<Material> CreateMaterialInstance() {
        static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
        std::shared_ptr<Material> material = std::make_shared<T>();
        mMaterials.push_back(material);
        return material;
    }

    std::vector<std::shared_ptr<Material>> MakeMaterials(uint8_t aBinding);

    static void DefaultPipelineConfigInfo(PipelineConfigInfo &aConfigInfo);

    const char *mPipelineName;
    PipelineConfigInfo mPipelineConfig;
    VkPipeline mGraphicsPipeline;
    std::vector<VkPipelineShaderStageCreateInfo> mShadersInPipeline;
    std::vector<std::shared_ptr<Material> > mMaterials = {};
    std::vector<Renderer *> mRenderers = {};
    VkPipelineDepthStencilStateCreateInfo mDepthStencil;
};
