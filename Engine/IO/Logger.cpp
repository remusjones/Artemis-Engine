#include "Logger.h"

#include "spdlog/spdlog.h"

void Logger::Log(spdlog::level::level_enum aSeverity, const char* aMessage)
{
    spdlog::set_level(aSeverity);
    spdlog::debug(aMessage);
}
