#include "Logger.h"
#include <iostream>

#include "spdlog/spdlog.h"

void Logger::Log(spdlog::level::level_enum aSeverity, const char* aMessage)
{
    spdlog::log(aSeverity, aMessage);
}

void Logger::Log(spdlog::level::level_enum aSeverity, const std::string &aMessage) {
    spdlog::log(aSeverity, aMessage.c_str());
}

void Logger::Log(const char *aMessage) {
    Log(spdlog::level::info, aMessage);
}

void Logger::Log(std::string aMessage) {
    Log(spdlog::level::info, aMessage);
}
