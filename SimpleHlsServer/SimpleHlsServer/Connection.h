#pragma once
#include <vector>
#include <mutex>
#include <queue>

class Connection {
public:
	Connection(int clientFd);
	~Connection();
public:
	int start();
private:

	int mClientFd;

};

