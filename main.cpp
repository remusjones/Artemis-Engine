#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "VulkanGraphicsImpl.h"
#include "glog/logging.h"

int main() {

    LOG(INFO) << "Initializing Application";
    IApplication* app = new VulkanGraphicsImpl("Engine", 1200, 800);
    try {
        app->Run();
    } catch (const std::exception& e) {
        LOG(ERROR) << e.what();
        return EXIT_FAILURE;
    }

    delete app;
    return EXIT_SUCCESS;
}

