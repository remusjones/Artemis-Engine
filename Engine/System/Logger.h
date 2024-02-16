#pragma once
#include "../../vendor/spdlog/include/spdlog/common.h"

class Logger {
public:
    // Placeholder until we get a proper log flow
    static void Log(spdlog::level::level_enum aSeverity, const char* aMessage);
    static void Log(spdlog::level::level_enum aSeverity, const std::string &aMessage);
    static void Log(const char* aMessage);
    static void Log(std::string aMessage);

};
