#ifndef _TASK_H
#define _TASK_H
#include<iostream>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string>
#include<errno.h>
#include<sys/epoll.h>
using namespace std;

const int buff_size = 1024;
void deletefd(int epollfd,int fd);//将连接文件符从epoll中删除

class Task
{
	private:
		int epoll_fd;
		int conn_fd;
	public:
		Task(int epoll,int fd):epoll_fd(epoll),conn_fd(fd){}
		~Task(){deletefd(epoll_fd,conn_fd);}
		void Done();
		int send_file(const string &filename,const string &type,int start,const int status =200,const string info="OK");
		int handle_get(const string &uri,int start =0);
		int get_filesize(const string &filename);
};




#endif
