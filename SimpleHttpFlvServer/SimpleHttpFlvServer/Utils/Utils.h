#pragma once


#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>

#define MODULE_NAME "SimpleHlsServer"
#define MODULE_VERSION "1.0"
#define MODULE_BUILD_DATE "2024-06-25"
#define MODULE_BUILD_TIME "11:00"
#define MODULE_BUILD_BRANCH "develop"
#define MODULE_BUILD_COMMIT_ID "null"
#define MODULE_BUILD_FIRMWARE_NAME "null"

namespace Utils {
class Version {
  public:
    static const char *GetName();
    static const char *GetVersion();
    static const char *GetBuildDate();
    static const char *GetBuildTime();
    static const char *GetBranch();
    static const char *GetCommitID();
    static const char *GetFirmwareName();
};

}  // namespace Utils
