#ifndef BOOSTSERVER_H
#define BOOSTSERVER_H

#include "boost.h"
#include <map>

class Config;
class HttpServerConnection;

class BoostServer
{
public:
	BoostServer(Config* config);
	~BoostServer();
public:
	void start();

	//启动后有客户端过来，使用该函数添加连接
	bool addConn(HttpServerConnection* conn);
	//删除连接
	bool removeConn(std::string session);
	//获取连接
	HttpServerConnection* getConn(std::string session);
	//发送数据
	bool sendData(char* data, int size);

	static void cbDisconnection(void* arg, std::string session);
	std::string generateSession();
private:
	void handleDisconnection(std::string session);

	//设置接收请求
	void setOnAccept();
	//setOnAccept的回调函数
	void onAccept(beast::error_code ec, tcp::socket socket);

private:
	Config* mConfig;
	net::io_context* mIoc;
	tcp::acceptor*   mAcceptor;
	//用map管理所有连接
	std::map<std::string, HttpServerConnection*> m_connMap;// <session,conn> 维护所有被创建的连接
	//互斥锁
	std::mutex								     m_connMap_mtx;

};


#endif //BOOSTSERVER_H