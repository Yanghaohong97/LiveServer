// SimpleHlsServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include "Utils/Log.h"
#pragma comment(lib, "ws2_32.lib")
#include "Connection.h"
#include <thread>

int main() {


	/*

	ffmpeg命令行生成m3u8切片（1）

	ffmpeg -i input.mp4 -c:v libx264 -c:a copy -f hls -hls_time 10 -hls_list_size 0 -hls_start_number 0 input/index.m3u8

	备注：-hls_time n: 设置每片的长度，默认值为2,单位为秒
		  -hls_list_size n:设置播放列表保存的最多条目，设置为0会保存有所片信息，默认值为5
		  -hls_start_number n:设置播放列表中sequence number的值为number，默认值为0
		  -hls_wrap n:设置多少片之后开始覆盖，如果设置为0则不会覆盖，默认值为0
			  这个选项能够避免在磁盘上存储过多的片，而且能够限制写入磁盘的最多的片的数量

	ffmpeg命令行生成m3u8切片（2）

	ffmpeg -i input.mp4 -fflags flush_packets
	-max_delay 2 -flags -global_header
	-hls_time 5 -hls_list_size 0 -vcodec libx264 -acodec aac -r 30 -g 60 index.m3u8

	*/
	//客户端请求：ffplay -i http://127.0.0.1:8080/index.m3u8


	int port = 8080;
	LOG_I("SimpleHlsServer http://127.0.0.1:%d/index.m3u8\n", port);

	//初始化WSA
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOG_E("WSAStartup error\n");
		return -1;
	}

	//创建套接字
	SOCKET serverFd;
	serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	//绑定IP和端口号
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//server_addr.sin_addr.s_addr = inet_addr("192.168.2.61");
	server_addr.sin_port = htons(port);
	if (bind(serverFd, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		LOG_E("socket bind error\n");
		return -1;
	}

	//开始监听
	if (listen(serverFd, SOMAXCONN) < 0) {
		LOG_E("socket listen error\n");
		return -1;
	}


	while (true)
	{
		LOG_I("等待新连接...\n");
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN accept_addr;
		int clientFd = accept(serverFd, (SOCKADDR*)&accept_addr, &len);
		//const char* clientIp = inet_ntoa(accept_addr.sin_addr);

		if (clientFd == SOCKET_ERROR) {
			LOG_E("accept connection error\n");
			break;
		}
		LOG_I("发现新连接 clientFd=%d\n", clientFd);

		//std::thread t([&]() {
		Connection conn(clientFd);
		conn.start();
		//});

		//t.detach();


	}

	closesocket(serverFd);
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
