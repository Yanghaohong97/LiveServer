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

	bool addConn(HttpServerConnection* conn);
	bool removeConn(std::string session);
	HttpServerConnection* getConn(std::string session);
	bool sendData(char* data, int size);

	static void cbDisconnection(void* arg, std::string session);
	std::string generateSession();
private:
	void handleDisconnection(std::string session);

	void setOnAccept();
	void onAccept(beast::error_code ec, tcp::socket socket);

private:
	Config* mConfig;
	net::io_context* mIoc;
	tcp::acceptor*   mAcceptor;
	std::map<std::string, HttpServerConnection*> m_connMap;// <session,conn> 维护所有被创建的连接
	std::mutex								     m_connMap_mtx;

};


#endif //BOOSTSERVER_H