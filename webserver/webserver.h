#ifndef _WEBSERVBER_H_
#define _WEBSERVBER_H_
#include "task.h"
#include "ThreadPool.h"
using namespace std;

const int MAX_EVENT_COUNT = 200;
class WebServer
{
	private:
		int port;
		int listenfd;
		int epollfd;
		struct sockaddr_in server;
	public:
		WebServer(int p):port(p),listenfd(0){memset(&server,0,sizeof(server));};
		~WebServer(){close(listenfd);};
		int start();
};


#endif
