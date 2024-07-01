#include "Server/BoostServer.h"
#include "Scheduler.h"
#include "Utils/Config.h"

/*
* 
* 实现一个高性能HTTP-FLV播放器：支持多并发
* 
* ffmpeg 播放http-flv视频流
    ffplay -i http://localhost:8080/test.flv

*/

int main(int argc, char* argv[])
{
    const char* file = NULL;
    file = "config.json";

    //用jsoncpp库解析config.json,获取ip、端口、线程数
    Config config(file);
    if (!config.state) {
        printf("failed to read config file: %s\n", file);
        return -1;
    }
    
    //实例化BoostServer
    BoostServer server(&config);
    //在子线程启动server
    std::thread([&]() {
        server.start();
    }).detach();
    
    Scheduler sch(&server,&config);
    sch.loop();

    return 0;
}
