#include "Logger.h"
#include <iostream>

#include <spdlog/spdlog.h>

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

void Logger::Log(const std::string &aMessage) {
    Log(spdlog::level::info, aMessage);
}

void Logger::LogWarning(const std::string &aMessage) {
    Log(spdlog::level::warn, aMessage);
}

void Logger::LogError(const char *str) {
    Log(spdlog::level::err, str);
}

void Logger::LogError(const std::string &aMessage) {
    Log(spdlog::level::err, aMessage);
}
