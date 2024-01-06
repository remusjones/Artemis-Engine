#pragma once
#include <memory>

#include "Material.h"


class DefaultMaterial : public Material {
public:
    enum BindingLocation {
        TEXTURE = 3
    };
    enum TextureComposite {
        ALBEDO = 0,
        NORMAL = 1
    };

    void Create(MaterialBase *aBaseMaterial = nullptr,
                const char *aMaterialName = "Unknown") override;

    void OnImGuiRender() override;

    void MakeDefaults();

    void Destroy() override;

    std::unique_ptr<Texture> mTextures;
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

    static constexpr const char* DefaultAssetPaths[] = {
        "/Assets/Textures/albedo_4096.png",
        "/Assets/Textures/normal_4096.png",
    };
};
