#pragma once
#include <memory>

#include "Base/Common/Material.h"


class DefaultMaterial : public Material {
public:

    enum BindingLocation {
        SCENE_INFORMATION = 0,
        PROPERTIES = 1,
        TEXTURE = 2
    };
    enum TextureComposite {
        ALBEDO = 0,
        NORMAL = 1
    };


    DefaultMaterial(const char* aMaterialName);
    DefaultMaterial() = default;


    void Create(MaterialBase *aBaseMaterial = nullptr) override;

    void OnImGuiRender() override;

    void MakeDefaults() const;

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
        "/Assets/Textures/Defaults/albedo_4096.png",
        "/Assets/Textures/Defaults/normal_4096.png",
    };
};
