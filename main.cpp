#include<iostream>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include "fun.h"
using namespace std;

int fd=-1;

void fun(int sig)
{
	do_exit(fd);
}

int main(int argc,char *argv[])
{
	//绑定信号处理函数（do_exit）;
	signal(SIGINT,fun);

	if(argc < 3)
	{
		cerr<<"cmd is not enough;errno:"<<errno<<endl;
		return 0;
	}

	//分离ip地址和端口号
	char *ip = argv[1];
	char* port = argv[2];

	//链接服务器
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == -1)
	{
		cerr<<"fd create fail;errno:"<<errno<<endl;
		return 0;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(port));
	saddr.sin_addr.s_addr = inet_addr(ip);

	if(-1 == connect(fd,(struct sockaddr*)&saddr,sizeof(saddr)))
	{
		cerr<<"connect fail;errno:"<<errno<<endl;
		return 0;
	}

	while(1)
	{
		//让用户选择服务
		cout<<"1.register   2.login    3.exit"<<endl;

		int a = 0;
		cout << "请输入选择服务：";
		cin >> a;
		switch(a)
		{
			case 1:
				{
					do_register(fd);
				}break;
			case 2:
				{
					do_login(fd);
				}break;
			case 3:
				{
					do_exit(fd);
				}break;
			default:
				cout<<"error"<<endl;
				break;
		}
		
	}
	
	return 0;
}
