//
// Created by Remus on 5/02/2024.
//

#pragma once
#include <string>

class AssetItem{
public:
    virtual ~AssetItem() = default;

    virtual void Create(std::string aPath) = 0;

    virtual std::string Description() const { return mDescription; }
    virtual std::string GetID() const { return mID; }
    virtual std::string GetPath() const { return mPath; }

protected:
    virtual void CreateFromAssetFile(std::string aPath) = 0;
    virtual void CreateFromMetaFile(std::string aPath) = 0;

    std::string mID;
    std::string mPath;
    std::string mDescription;
};
