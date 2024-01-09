#include "MaterialManager.h"

void MaterialManager::AddMaterial(const std::shared_ptr<Material> &aMaterial) {
    mMaterials.push_back(aMaterial);
}

Material* MaterialManager::MakeMaterials() const {
    for (const auto& material: mMaterials) {
        material->Create(material.get());
    }
    return mMaterials[0].get();
}
