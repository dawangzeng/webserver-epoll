#include<iostream>
#include<cstdio>
#include<string>
#include<cstring>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
using namespace std;
const char *user = "fanglin";
const char *pwd = "123456";
int main(int argc,char *argv[])
{
	char username[30];
	char password[30];
	int conn_fd;
	int i=0;
	for(;i<argc;i++)
		cout<<argv[i]<<endl;
	sscanf(argv[0],"%s %s %d",username,password,&conn_fd);
	cout<<"login:username:"<<username<<" password:"<<password<<" connfd:"<<conn_fd<<endl;
	string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
	string body = "<html><head><title>Login</title></head>";
	if(strcmp(user,username) == 0 && strcmp(pwd,password) == 0)
		body += "<body><p>Login Success!";
	else
		body += "<body><p>Login Failed!";
	body += "</p></body></html>";
	header += body;
	send( conn_fd, header.c_str(), header.size(), 0);
	return 0;
}
