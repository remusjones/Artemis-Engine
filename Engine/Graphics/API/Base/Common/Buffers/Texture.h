//
// Created by Remus on 27/12/2023.
//

#pragma once
#include "AllocatedImage.h"
struct Texture {
    AllocatedImage image;
    VkImageView imageView;
};
