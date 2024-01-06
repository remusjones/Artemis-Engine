//
// Created by Remus on 6/01/2024.
//

#pragma once
#include <memory>

#include "Common/Material.h"


class DefaultMaterialTest : public Material {
public:
    enum TextureBinding {
        TEXTURE = 3
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
};
