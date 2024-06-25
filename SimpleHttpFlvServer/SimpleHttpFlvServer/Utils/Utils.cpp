#include "Utils.h"
#include <cstdio>
#include <thread>
#include <future>
#include "Log.h"
namespace Utils {
const char *Version::GetName() {
    return MODULE_NAME;
}

const char *Version::GetVersion() {
    return MODULE_VERSION;
}

const char *Version::GetBuildDate() {
    return MODULE_BUILD_DATE;
}

const char *Version::GetBuildTime() {
    return MODULE_BUILD_TIME;
}

const char *Version::GetBranch() {
    return MODULE_BUILD_BRANCH;
}

const char *Version::GetCommitID() {
    return MODULE_BUILD_COMMIT_ID;
}

const char *Version::GetFirmwareName() {
    return MODULE_BUILD_FIRMWARE_NAME;
}


}  // namespace Utils
