#ifndef _BALANCER_
#define _BALANCER_
#include  <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
#include "../memory/ngx_pool.h"
#include "../log/Log.h"
#include "hashcirle.h"
using namespace std;
struct fds
{
	int epollfd;
	int sockfd;
};
class Balancer
{
	public:
	Balancer(const char *ip,unsigned short port);
	bool LoadEtcSer(const char *etcpath);
	void InitServers();
	int Accept();
	int RecvCli(int fd);
	void RecvSer(int fd);
	void Run();
	private:
	HashCon hc;
	Logger *log;
	map<string,int>mConser;
	vector<int> vec;
	int mListenfd;
	char* mIp;
	unsigned short mport;
	multimap<string,unsigned short> mSerMap;
	static int count;
	friend void* worker(void *arg);
	
};
#endif
