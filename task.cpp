#include "task.h"

void deletefd(int epollfd,int fd)
{
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL);
	close(fd);
}

void Task::Done()
{
	char buff[buff_size]={0};
	int ret;
	while(ret = recv(conn_fd,buff,1024,0))
	{
		if(!ret)
		{
			cout<<"浏览器退出"<<endl;
			close(conn_fd);
		}else if(ret<0)
		{
			continue;
		}

		stringstream ss;
		ss<<buff;
		string method,uri,version;
		ss>>method>>uri>>version;
		cout<<"method:"<<method<<endl;
		cout<<"uri:"<<uri<<endl;
		cout<<"version:"<<version<<endl;

		if(method == "GET")
			handle_get(uri);
		else
		{
			string status="HTTP/1.1 501 Not Implemented\r\nContent-Type: text/plain;charset=utf-8\r\n\r\n";
			send(conn_fd,status.c_str(),status.size(),0);
		}

		break;//退出循环
	}
	close(conn_fd);//关闭连接套接字，虽然析构函数也执行了
}

int Task::handle_get(const string &uri,int start)
{
	string filename = uri.substr(1);
	if(uri == "/" || uri == "/index.html")
		send_file("index.html","text/html",start);
	else if(uri.find(".jpg") != string::npos || uri.find(".png") != string::npos)
		send_file(filename,"image/jpg",start);
	else if(uri.find(".html") != string::npos)
		send_file(filename,"text/html",start);
	else 
		send_file(filename,"text/plain",start);

	return 0;
}
int Task::send_file(const string &filename,const string &type,int start,const int status,const string info)
{
	struct stat filestat;
	int ret = stat(filename.c_str(),&filestat);
	if(ret<0 || !S_ISREG(filestat.st_mode)) //打开出错，或者没有该文件
	{
		cout<<"file not found:"<<filename<<endl;
		send_file("html/404.html","text/html",0,404,"NOT FOUND");
		return -1;
	}
	char header[100];
	sprintf(header,"HTTP/1.1 %d %s\r\nServer: funnyserver\r\nContent-Length: %d\r\nContent-Type: %s;charset:utf-8\r\n\r\n", status, info.c_str(), int(filestat.st_size - start), type.c_str());
	send(conn_fd,header,strlen(header),0);
	int sum = start;
	int fd = open(filename.c_str(),O_RDONLY);
	while(sum < filestat.st_size)
	{
		off_t ft = sum;
		ret = sendfile(conn_fd,fd,&ft,filestat.st_size);
		if(ret<0)
		{
			cerr<<"erron:"<<errno<<",ret:"<<ret<<endl;
			if(errno == EAGAIN)
			{
				cerr<<"error is EAGAIN"<<endl;
				continue;//继续发送文件
			}
			else
			{
				cerr<<"other error"<<endl;
				close(fd);
				return -1;
			}
		}
		sum+=ret;
	}
	close(fd);
	return 0;
}

