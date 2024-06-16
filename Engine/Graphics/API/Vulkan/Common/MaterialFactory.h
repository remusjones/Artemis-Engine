//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>

#include "Base/Common/Material.h"


class MaterialFactory {
public:
    template<typename T>
    T* Create(const char* aMaterialName = "Default") {
        static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
        std::unique_ptr<T> material = std::make_unique<T>(aMaterialName);
        T* materialPtr = material.get();
        mMaterials.push_back(std::move(material)); // move into array
        return materialPtr;
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

    std::vector<std::unique_ptr<Material>> mMaterials = {};
};
