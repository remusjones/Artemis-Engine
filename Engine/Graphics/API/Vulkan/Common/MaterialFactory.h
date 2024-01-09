//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>

#include "Base/Common/Material.h"


class MaterialFactory {
public:
    template<typename T>
    std::shared_ptr<Material> CreateMaterialInstance(const char* aMaterialName = "Default") {
        static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
        std::shared_ptr<Material> material = std::make_shared<T>(aMaterialName);
        mMaterials.push_back(material);
        return material;
    }

    void MakeMaterials() const {
        for (const auto& material: mMaterials) {
            material->Create(material.get());
        }
    }
    void DestroyMaterials() const {
        for(const auto& material : mMaterials) {
            material->Destroy();
        }
    }

    [[nodiscard]] std::vector<VkDescriptorSetLayout> GetDescriptorLayouts() const {
        std::vector<VkDescriptorSetLayout> descriptors(mMaterials.size());

        for(int i = 0; i < mMaterials.size(); i++)
            descriptors[i] = mMaterials[i]->GetDescriptorLayout();

        return descriptors;
    }

    std::vector<std::shared_ptr<Material>> mMaterials = {};
};
