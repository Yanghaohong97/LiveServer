#include "Connection.h"
#include <iostream>
#include <string>
#include <thread>
#include "Utils/Log.h"
#include "Utils/Utils.h"

#pragma warning(disable: 4996)
#include <WinSock2.h>
#include <WS2tcpip.h>

char buf[1500000];

Connection::Connection(int clientFd) :mClientFd(clientFd) {
    LOG_I("clientFd=%d", clientFd);
}
Connection::~Connection() {
    LOG_I("clientFd=%d", mClientFd);
    closesocket(mClientFd);

}
int Connection::start() {

    char bufRecv[2000] = { 0 };         // 用于接收从客户端发来的数据
    int bufRecvSize = recv(mClientFd, bufRecv, 2000, 0);        // 将数据存储到 bufRecv 中
    LOG_I("bufRecvSize=%d,bufRecv=%s", bufRecvSize, bufRecv);    // 用于存储从 HTTP 请求中解析出的 URI

    char uri[100] = { 0 };// /, /index0.ts, /index1.ts ,,,

    const char* sep = "\n";             // 用于分割 HTTP 请求的不同行
    char* line = strtok(bufRecv, sep);  // 将接收到的 HTTP 请求按照 \n 分割成不同的行，并从中提取出第一行
    while (line) {
        if (strstr(line, "GET")) {      // 检查当前行是否包含 "GET"
            if (sscanf(line, "GET %s HTTP/1.1\r\n", &uri) != 1) {   // 解析出 HTTP 请求中的 URI，并将其存储到 uri 中
                LOG_E("parse uri error");

            }
        }
        line = strtok(NULL, sep);
    }
    printf("uri=%s\n", uri);

    std::string filename = "../ffmpeg/test" + std::string(uri);       // 根据解析出的 URI 构建文件路径
    FILE* fp;
    fp = fopen(filename.data(), "rb");  // 打开相应的文件，准备读取数据
    if (!fp) {
        LOG_E("fopen %s error", filename.data());
        return -1;
    }

    int bufLen = fread(buf, 1, sizeof(buf), fp);    // 从文件中读取数据到缓冲区 buf 中，并记录读取的数据长度

    LOG_I("bufLen=%d", bufLen);
    if (fp) {
        fclose(fp);
    }

    char http_headers[2000];             // 构建 HTTP 响应头部

    if (0 == strcmp("/index.m3u8", uri)) {
        sprintf(http_headers, "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: * \r\n"   // 让客户端能够跨域访问资源
            "Connection: keep-alive\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: application/vnd.apple.mpegurl; charset=utf-8\r\n"
            "Keep-Alive: timeout=30, max=100\r\n"
            "Server: hlsServer\r\n"
            "\r\n",
            bufLen);
    }
    else {
        sprintf(http_headers, "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: * \r\n"
            "Connection: close\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: video/mp2t; charset=utf-8\r\n"
            "Keep-Alive: timeout=30, max=100\r\n"
            "Server: hlsServer\r\n"
            "\r\n",
            bufLen);
    }

    int http_headers_len = strlen(http_headers);
    LOG_I("http_headers_len=%d", http_headers_len);

    send(mClientFd, http_headers, http_headers_len, 0);
    send(mClientFd, buf, bufLen, 0);

    Sleep(10);

    return 0;

}


