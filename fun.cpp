#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <json/json.h>
#include <string.h>
#include"fun.h"
#include"public.h"
using namespace std;

void *pth_run(void *arg)
{
	char buff[512] = {0};

	int fd = (int)arg;
	//接收数据
	while(1)
	{
		if(0 < recv(fd,buff,99,0))
		{
			cout<<buff<<endl;
			//break;
		}
		memset(buff,0,99);
	}
	pthread_exit(NULL);
	return 0;
}

void do_register(int fd)
{
	//让用户输入name   pw
	char name[128] = {0};
	cout<<"please input name:";
	cin >> name;

	char pw[20] = {0};
	cout<<"please input password:";
	cin >> pw;
		
	//JSON 打包数据
	Json::Value val;
	val["reason_type"] = MSG_TYPE_REGISTER;
	val["name"] = name;
	val["pw"] = pw;

	//发送到服务器
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return ;
	}
	//接受服务器消息（判断success/error）
				//如果注册成功
				//return
	pthread_t pth;
	if(-1 == pthread_create(&pth, NULL, pth_run, (void*)fd))
	{
		cerr<<"pthread_create fali;errno:"<<errno<<endl;
		return ;
	}
}

void do_login(int fd)
{
	//用户输入name pw
	char name[128] = {0};
	cout<<"please input name:"<<endl;
	cin >> name;

	char pw[20] = {0};
	cout<<"please input password:"<<endl;
	cin >> pw;

	//Json打包数据
	Json::Value val;
	val["reason_type"] = MSG_TYPE_LOGIN;
	val["name"] = name;
	val["pw"] = pw;

	//发送
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return ;
	}

	char buff[100] = {0};
	//接受服务器回应（判断success/error）
	if(0 < recv(fd,buff,99,0))
	{cout<<buff<<endl;
		if(strcmp(buff,"login success!")==0)
		{
			while(1)
			{
				pthread_t pth;
				if(-1 == pthread_create(&pth, NULL, pth_run, (void*)fd))
				{
					cerr<<"pthread_create fali;errno:"<<errno<<endl;
					return ;
				}

				cout<<"1. get list  2. talk to one  3.talk to group  4.exit"<<endl;
				cout<<"请输入操作序号：";
				int a = 0;
				cin >> a;
				switch(a)
				{
					case 1:
						{
							do_get_list(fd);		
						}break;
					case 2:
						{
							do_talk_to_one(fd);
						}break;
					case 3:
						{
							do_talk_to_group(fd);
						}break;
					case 4:
						{
							do_exit(fd);
							goto out;
						}break;
					default:
						cout<<"error"<<endl;break;
				}
			}
		}
		else if(strcmp(buff,"name not find!")==0)
		{
			cout<<"该用户没有注册！"<<endl;
		}
		else if(strcmp(buff,"pw errno!")==0)
		{
			cout<<"密码输入错误！"<<endl;
		}
		else if(strcmp(buff,"login fail!")==0)
		{
			cout<<"该用户已登录！"<<endl;
		}
	
	}
	else
	{
		cout<<"登录失败！"<<endl;
	}
	out:;
}


void do_get_list(int fd)
{
	Json::Value val;
	val["reason_type"] = MSG_TYPE_GET_LIST;

	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return ;
	}
	pthread_t pth;
	if(-1 == pthread_create(&pth, NULL, pth_run, (void*)fd))
	{
		cerr<<"pthread_create fali;errno:"<<errno<<endl;
		return ;
	}
	
}


void do_talk_to_one(int fd)
{
	char name[128] = {0};
	cout<<"input send name:"<<endl;
	cin >> name;
	
	char message[128] = {0};
	cout<<"input message:"<<endl;
	cin.ignore();
	cin.getline(message,128);


	Json::Value val;
	val["reason_type"] = MSG_TYPE_TALK_TO_ONE;
	val["name"] = name;
	val["message"] = message;

	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return ;
	}

}

void do_talk_to_group(int fd)
{
	char allname[1024] = {0};
	do_get_list(fd);
	cout << "请输入想发送消息的用户:(逗号分隔用户名)";
	cin >> allname;

	char msg[100] = {0};
	cout << "input message:";
	cin >> msg;

	char *name = strtok(allname,",");
	while(name)
	{
		
		Json::Value val;
		val["reason_type"] = MSG_TYPE_TALK_TO_GROUP;
		val["name"] = name;
		val["message"] = msg;

		if(-1 == send(fd,val.toStyledString().c_str()
					,strlen(val.toStyledString().c_str()),0))
		{
			cerr << "send user fail;errno:" << errno << endl;
			return ;
		}
		name = strtok(NULL,",");
	}
}


void do_exit(int fd)
{
	char username[100] = {0};
	cout<<"请输入您要下线的用户名:";
	cin >> username;

	Json::Value val;
	val["reason_type"] = MSG_TYPE_EXIT;
	val["name"] = username;
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send exit fail;errno:"<<errno<<endl;
		return ;
	}
	close(fd);
}
