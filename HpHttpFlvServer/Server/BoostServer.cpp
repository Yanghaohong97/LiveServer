#include "BoostServer.h"
#include "HttpServerConnection.h"
#include "../Utils/Config.h"
#include "../Utils/Log.h"

BoostServer::BoostServer(Config* config) : mConfig(config)
{

}
BoostServer::~BoostServer() {
	//销毁资源
	if (mAcceptor) {
		delete mAcceptor;
		mAcceptor = nullptr;
	}
	if (mIoc) {
		delete mIoc;
		mIoc = nullptr;
	}
}
void BoostServer::start()
{
	//根据config配置在启动的时候指定线程数
	int threadNum = std::max<int>(1, mConfig->getThreadNum());
	unsigned short port = mConfig->getPort();
	LOGI("BoostServer::start() ip=%s,port=%d,threadNum=%d", mConfig->getIp() , port, threadNum);

	//转成boost库的地址（包括IP、IPV4/IPV6协议等）
	boost::asio::ip::address address = net::ip::make_address(mConfig->getIp());
	tcp::endpoint endpoint{ address, port };

	mIoc = new net::io_context{ threadNum };
	mAcceptor = new tcp::acceptor(*mIoc);

	beast::error_code ec;
	//判断是否支持地址对应的协议
	mAcceptor->open(endpoint.protocol(), ec);
	if (ec)
	{
		LOGE("acceptor.open error: %s", ec.message().data());
		return;
	}
	//设置地址重用
	mAcceptor->set_option(net::socket_base::reuse_address(true), ec);
	if (ec)
	{
		LOGE("acceptor.set_option error: %s", ec.message().data());
		return;
	}
	//绑定
	mAcceptor->bind(endpoint, ec);
	if (ec)
	{
		LOGE("acceptor.bind error: %s", ec.message().data());
		return;
	}
	//监听
	mAcceptor->listen(net::socket_base::max_listen_connections, ec);
	if (ec)
	{
		LOGE("acceptor.listen error: %s", ec.message().data());
		return;
	}
	//设置接收请求
	setOnAccept();

	//创建线程容器，方便管理
	std::vector<std::thread> ts;
	ts.reserve(threadNum - 1);
	//用io_context按配置启动多线程
	for (auto i = 0; i < threadNum - 1; ++i)
	{
		ts.emplace_back([this] {
			std::thread::id threadId = std::this_thread::get_id();
			LOGI("ioc sub threadId=%d", threadId);
			mIoc->run();
		});
	}
	std::thread::id threadId = std::this_thread::get_id();
	LOGI("ioc main threadId=%d", threadId);
	mIoc->run();

}

void BoostServer::setOnAccept()
{
	//异步接收请求，接收成功后回调onAccept
	mAcceptor->async_accept(net::make_strand(*mIoc),
		beast::bind_front_handler(&BoostServer::onAccept, this));
}
void BoostServer::onAccept(beast::error_code ec, tcp::socket socket)
{
	if (ec)
	{
		LOGE("onAccept error: %s", ec.message().data());
		return;
	}
	else
	{
		//创建新连接，添加连接，然后设置断连回调，开始运行
		HttpServerConnection* conn = new HttpServerConnection(this, socket);
		if (this->addConn(conn)) {
			conn->setDisconnectionCallback(BoostServer::cbDisconnection, this);
			conn->run();
		}
		else {
			delete conn;
			conn = nullptr;
		}

	}
	//重新触发接收请求
	setOnAccept();
}

bool BoostServer::addConn(HttpServerConnection* conn) {
	m_connMap_mtx.lock();
	if (m_connMap.find(conn->getSession()) != m_connMap.end()) {
		m_connMap_mtx.unlock();
		return false;
	}
	else {
		m_connMap.insert(std::make_pair(conn->getSession(), conn));
		m_connMap_mtx.unlock();
		return true;
	}
}
bool BoostServer::removeConn(std::string session) {
	m_connMap_mtx.lock();
	std::map<std::string, HttpServerConnection*>::iterator it = m_connMap.find(session);
	if (it != m_connMap.end()) {
		m_connMap.erase(it);
		m_connMap_mtx.unlock();
		return true;
	}
	else {
		m_connMap_mtx.unlock();
		return false;
	}
}
HttpServerConnection* BoostServer::getConn(std::string session) {

	m_connMap_mtx.lock();
	std::map<std::string, HttpServerConnection*>::iterator it = m_connMap.find(session);
	if (it != m_connMap.end()) {
		m_connMap_mtx.unlock();
		return it->second;
	}
	else {
		m_connMap_mtx.unlock();
		return nullptr;
	}
}
void BoostServer::cbDisconnection(void* arg, std::string session) {
	BoostServer* server = (BoostServer*)arg;
	server->handleDisconnection(session);

}
bool BoostServer::sendData(char* data, int size) {
	bool result = false;

	m_connMap_mtx.lock();
	std::map<std::string, HttpServerConnection*>::iterator it;
	if (m_connMap.size() > 0) {
		result = true;
	}
	//LOGI("实时在线客户端数量=%lld,size=%d", m_connMap.size(),size);

	for (it = m_connMap.begin(); it != m_connMap.end(); it++) {
		HttpServerConnection* conn = it->second;
	}
	m_connMap_mtx.unlock();
	return result;
}

void BoostServer::handleDisconnection(std::string session) {
	LOGI("session=%s,disconnection", session.data());
	HttpServerConnection* conn = this->getConn(session);
	this->removeConn(session);
	if (conn) {
		delete conn;
		conn = nullptr;
	}


}
std::string BoostServer::generateSession()
{
	std::string numStr;
	numStr.append(std::to_string(rand() % 9 + 1));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	numStr.append(std::to_string(rand() % 10));
	//int num = stoi(numStr);

	return numStr;
}