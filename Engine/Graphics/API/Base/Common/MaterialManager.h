#pragma once
#include <memory>
#include <vector>

#include "Material.h"

class Material;

class MaterialManager {
public:

    void AddMaterial(const std::shared_ptr<Material> &aMaterial);
    Material *MakeMaterials() const;
    std::vector<std::shared_ptr<Material>> mMaterials;
};
