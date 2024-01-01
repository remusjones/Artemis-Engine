#include "Logger.h"
#include <iostream>

#include "spdlog/spdlog.h"

void Logger::Log(spdlog::level::level_enum aSeverity, const char* aMessage)
{
    spdlog::log(aSeverity, aMessage);
}
