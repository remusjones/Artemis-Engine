#pragma once
#include <memory>

#include "Material.h"


class DefaultMaterial : public Material {
public:
    enum TextureBinding {
        ALBEDO = 3,
        NORMAL = 4
    };

    void Create(MaterialBase *aBaseMaterial = nullptr,
                const char *aMaterialName = "Unknown") override;

    void OnImGuiRender() override;

    void MakeDefaults();

    void Destroy() override;

    std::unique_ptr<Texture> mDefaultAlbedo;
    std::unique_ptr<Texture> mDefaultNormal;

    std::vector<const char *> mDebugColors{
        "Default",
        "normal",
        "lightDirection",
        "viewDirection",
        "reflectionDirection",
        "ambientColor",
        "diffuseColor",
        "specularColor",
        "texColor",
        "inUV",
    };
};
