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

    char bufRecv[2000] = { 0 };         // ���ڽ��մӿͻ��˷���������
    int bufRecvSize = recv(mClientFd, bufRecv, 2000, 0);        // �����ݴ洢�� bufRecv ��
    LOG_I("bufRecvSize=%d,bufRecv=%s", bufRecvSize, bufRecv);    // ���ڴ洢�� HTTP �����н������� URI

    char uri[100] = { 0 };// /, /index0.ts, /index1.ts ,,,

    const char* sep = "\n";             // ���ڷָ� HTTP ����Ĳ�ͬ��
    char* line = strtok(bufRecv, sep);  // �����յ��� HTTP ������ \n �ָ�ɲ�ͬ���У���������ȡ����һ��
    while (line) {
        if (strstr(line, "GET")) {      // ��鵱ǰ���Ƿ���� "GET"
            if (sscanf(line, "GET %s HTTP/1.1\r\n", &uri) != 1) {   // ������ HTTP �����е� URI��������洢�� uri ��
                LOG_E("parse uri error");

            }
        }
        line = strtok(NULL, sep);
    }
    printf("uri=%s\n", uri);

    std::string filename = "../ffmpeg/test" + std::string(uri);       // ���ݽ������� URI �����ļ�·��
    FILE* fp;
    fp = fopen(filename.data(), "rb");  // ����Ӧ���ļ���׼����ȡ����
    if (!fp) {
        LOG_E("fopen %s error", filename.data());
        return -1;
    }

    int bufLen = fread(buf, 1, sizeof(buf), fp);    // ���ļ��ж�ȡ���ݵ������� buf �У�����¼��ȡ�����ݳ���

    LOG_I("bufLen=%d", bufLen);
    if (fp) {
        fclose(fp);
    }

    char http_headers[2000];             // ���� HTTP ��Ӧͷ��

    if (0 == strcmp("/index.m3u8", uri)) {
        sprintf(http_headers, "HTTP/1.1 200 OK\r\n"
            "Access-Control-Allow-Origin: * \r\n"   // �ÿͻ����ܹ����������Դ
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


