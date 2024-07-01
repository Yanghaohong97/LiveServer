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

	//�������пͻ��˹�����ʹ�øú����������
	bool addConn(HttpServerConnection* conn);
	//ɾ������
	bool removeConn(std::string session);
	//��ȡ����
	HttpServerConnection* getConn(std::string session);
	//��������
	bool sendData(char* data, int size);

	static void cbDisconnection(void* arg, std::string session);
	std::string generateSession();
private:
	void handleDisconnection(std::string session);

	//���ý�������
	void setOnAccept();
	//setOnAccept�Ļص�����
	void onAccept(beast::error_code ec, tcp::socket socket);

private:
	Config* mConfig;
	net::io_context* mIoc;
	tcp::acceptor*   mAcceptor;
	//��map������������
	std::map<std::string, HttpServerConnection*> m_connMap;// <session,conn> ά�����б�����������
	//������
	std::mutex								     m_connMap_mtx;

};


#endif //BOOSTSERVER_H