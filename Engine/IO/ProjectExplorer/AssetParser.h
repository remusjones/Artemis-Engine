//
// Created by Remus on 7/02/2024.
//

#pragma once
#include <string>
#include <unordered_map>


class AssetItem;

class AssetParser {
public:
    void Parse(std::string aPath);
    void RegisterAssetType(std::string aExtension, AssetItem aAssetItem);

private:
    std::unordered_map<std::string, AssetItem> mAssociatedAssetTypes;
};
