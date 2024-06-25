#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Utils.h"

// extern int enable_minilog;
// extern int log_level;
int enable_minilog = 0;
int log_level = LOG_LEVEL_DEBUG;

extern std::string logLevelToString(int level);
extern int getLogLevel();

void setLogLevel(int level)
{
    LOG_I("set log_level to:%d\n", log_level);
    log_level = level;
}

int getLogLevel()
{
    return log_level;
}

std::string logLevelToString(int level) {
    std::string levelStr("INFO");
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        levelStr = "DEBUG";
        break;
    case LOG_LEVEL_INFO:
        levelStr = "INFO";
        break;
    case LOG_LEVEL_WARN:
        levelStr = "WARN";
        break;
    case LOG_LEVEL_ERROR:
        levelStr = "ERROR";
        break;
    default:
        break;
    }
    return levelStr;
}