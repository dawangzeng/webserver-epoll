#include "webserver.h"
#include "task.h"
int main(int argc,char * argv[])
{
	if(argc<2 || argv == nullptr)
	{
		cerr<<"arguments is not enough"<<endl;
		exit(-1);
	}
	int port = atoi(argv[1]);
	WebServer server(port);
	server.start();
	return 0;
}
