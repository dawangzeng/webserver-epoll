#include "webserver.h"
#include<signal.h>
//设置套接字为非阻塞状态
void setnoblocking(int fd){
	int old_op =  fcntl(fd,F_GETFL);
	int new_op = old_op | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_op);
}

//添加到epoll中
void addepoll(int epollfd,int fd){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLONESHOT | EPOLLET | EPOLLRDHUP;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnoblocking(fd);
}

int WebServer::start()
{
	signal(SIGPIPE, SIG_IGN);
	//初始化服务地址
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	//创建套接字
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd < 0)
	{
		cerr<<"sock failed!"<<endl;
		exit(-1);
	}
	//绑定套接字
	int ret = bind(listenfd,(struct sockaddr *)&server,sizeof(server));
	if(ret < 0)
	{
		cerr<<"bind error!"<<endl;
		exit(-1);
	}
	//监听套接字
	ret = listen(listenfd,200);
	if(ret<0)
	{
		cerr<<"listen error"<<endl;
		exit(-2);
	}
	cout<<"server listening...."<<endl;

	//创建线程池
	ThreadPool<Task> pool(100);
	//创建epoll
	epollfd = epoll_create(2000);
	if( epollfd < 0  ) {
		 cout << "epoll_create error, line: " << __LINE__ << endl;
		 exit(-1);
						    
	}
	epoll_event events[MAX_EVENT_COUNT];
	//将监听套接字加入到epoll中
	epoll_event event;
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLHUP;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event);


	//循环处理
	for(;;)
	{
		//epoll等待事件
		ret = epoll_wait(epollfd,events,MAX_EVENT_COUNT,-1);
		if(ret < 0)
		{
			cerr<<"wait error!"<<endl;
			exit(-1);
		}
		for(int i = 0; i < ret ;i++)
		{
			int fd = events[i].data.fd;
			if(listenfd == fd)//有新的连接
			{
				struct sockaddr_in client;
				socklen_t client_size = sizeof(client);
				int conn = accept(fd,(struct sockaddr *)&client,&client_size);
				if(conn < 0)
				{
					cerr<<"accept error!"<<endl;
					continue;
				}
				cout<<"new connection.."<<endl;
				addepoll(epollfd,conn);
			}else if(events[i].events &(EPOLLHUP | EPOLLRDHUP |EPOLLERR))
			{
				deletefd(epollfd,fd);
			}else if(events[i].events & EPOLLIN)//有数据可以读
			{
				cout<<"epollin"<<endl;
				Task *task =  new Task(epollfd,fd);
				cout<<"pool add.. "<<fd<<endl;
				pool.append(task);
			}else
			{
				cerr<<"Unknown"<<endl;
			}
		}

	}
	return 0;


}

