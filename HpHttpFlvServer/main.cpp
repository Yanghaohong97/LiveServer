#include "Server/BoostServer.h"
#include "Scheduler.h"
#include "Utils/Config.h"

/*
ffmpeg 播放http-flv视频流
    ffplay -i http://localhost:8080/test.flv

*/

int main(int argc, char* argv[])
{
    const char* file = NULL;
    file = "config.json";

    Config config(file);
    if (!config.state) {
        printf("failed to read config file: %s\n", file);
        return -1;
    }
    
    BoostServer server(&config);
    std::thread([&]() {
        server.start();
    }).detach();
    
    Scheduler sch(&server,&config);
    sch.loop();

    return 0;
}
