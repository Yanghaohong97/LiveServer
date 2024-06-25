// SimpleHttpFlvServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdint.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)  //禁用fopen的警告选项_CRT_SECURE_NO_WARNINGS


int main() {

	int port = 8080;
	const char* filename = "../data/test.flv";

	printf("httpflvServer http://127.0.0.1:%d/test.flv\n", port);

	//初始化WSA
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup error\n");
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
		printf("socket bind error\n");
		return -1;
	}

	//开始监听
	if (listen(serverFd, SOMAXCONN) < 0) {
		printf("socket listen error\n");
		return -1;
	}


	constexpr char http_headers[] = \
		"HTTP/1.1 200 OK\r\n" \
		"Access-Control-Allow-Origin: * \r\n" \
		"Content-Type: video/x-flv\r\n" \
		"Content-Length: -1\r\n" \
		"Connection: Keep-Alive\r\n" \
		"Expires: -1\r\n" \
		"Pragma: no-cache\r\n" \
		"\r\n"
		;
	//"Content-Type: video/x-flv\r\n":因为数据是flv格式
	//"Content-Length: -1\r\n":这是区别HLS的，因为Http-flv一直发送数据，所以长度定义为-1
	//"Connection: Keep-Alive\r\n" :因为Http-flv一直发送数据.所以要保持不断开

	int http_headers_len = strlen(http_headers);

	/*

constexpr char http_headers[] = \
"HTTP/1.1 200 OK\r\n" \
"Access-Control-Allow-Origin: * \r\n" \
"Cache-Control: no-cache\r\n" \
"Content-Type: video/x-flv\r\n" \
"Connection: close\r\n" \
"Expires: -1\r\n" \       //设置资源的有效期来控制http的缓存
"Pragma: no-cache\r\n" \  //用于客户端发送的请求中。客户端会要求所有的中间服务器不返回缓存的资源
"\r\n"
;
	*/

	while (true)
	{
		printf("等待新连接...\n");
		int len = sizeof(SOCKADDR);
		SOCKADDR_IN accept_addr;
		int clientFd = accept(serverFd, (SOCKADDR*)&accept_addr, &len);
		//const char* clientIp = inet_ntoa(accept_addr.sin_addr);

		if (clientFd == SOCKET_ERROR) {
			printf("accept connection error\n");
			break;
		}
		printf("发现新连接 clientFd=%d\n", clientFd);
		unsigned char buf[5000];

		char bufRecv[2000] = { 0 };


		//https://blog.csdn.net/qq_32331073/article/details/82148409
		FILE* fp;
		fp = fopen(filename, "rb");
		//if (fopen_s(&fp, filename, "r") != 0) {
		//	printf("fopen %s fail!\n", filename);
		//}
		
		if (!fp) {
			printf("fopen %s fail!\n", filename);
		}
		else {

			int times = 0;
			while (true) {
				times++;

				//第一次的时候发送http_headers，然后就可以开始源源不断发送flv数据
				if (times == 1) {
					int bufRecvSize = recv(clientFd, bufRecv, 2000, 0);
					printf("bufRecvSize=%d,bufRecv=%s\n", bufRecvSize, bufRecv);

					send(clientFd, http_headers, http_headers_len, 0);

				}
				else {
					//usleep(1000 * 100);
					Sleep(10);
					int bufLen = fread(buf, 1, sizeof(buf), fp);
					//                        fseek(srcFile, 0, SEEK_CUR);
					int ret = send(clientFd, (char*)buf, bufLen, 0);

					//直到读取flv文件的数据为空后，退出循环，结束发送
					if (ret <= 0) {
						break;
					}
					else {
						//printf("send bufLen=%d,ret=%d \n", bufLen, ret);
					}

				}

			}
		}

		if (fp) {
			fclose(fp);
		}
		closesocket(clientFd);
		printf("关闭连接 clientFd=%d\n", clientFd);
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
