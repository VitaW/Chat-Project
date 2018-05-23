#include <iostream>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <errno.h>
#include <assert.h>
#include <mysql/mysql.h>
#include <sys/socket.h>
#include "talkone.h"
#include "mysql.h"
using namespace std;
extern Mysql Mysql_sever;



void View_TalkOne::process(int fd,char* json)
{
	cout<<"View_talkone:process run"<<endl;
	_fd = fd;
	int tofd = 0;
	Json::Value root;
	Json::Reader read;
	if(-1 == read.parse(json,root))
	{
		cerr<<"json parse fail;errno:"<<errno<<endl;
		return ;
	}

	char name[100] = {0};
	strcpy(name,root["name"].asString().c_str());cout<<name<<":";
	char message[100] = {0};
 	strcpy(message,root["message"].asString().c_str());cout<<message<<endl;

	char cmd1[1024] = "SELECT * FROM user WHERE name='";
	strcat(cmd1,name);
	strcat(cmd1,"';");
	cout<<cmd1<<endl;
	if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))
	{
		cerr<<"select one fail;errno:"<<errno<<endl;
		return ;
	}

	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);	
	if(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))
	{
		char cmd2[100] = "SELECT * FROM online WHERE name='"; 
		sprintf(cmd2+strlen(cmd2),"%s",name);
		sprintf(cmd2+strlen(cmd2),"%s","');");

		if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
		{
			cerr << "select fail;errno:"<<errno<<endl;
			return ;
		}
		Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);	
		if(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))//如果该用户在线
		{
			tofd = atoi(Mysql_sever._mp_row[0]);    
	cout<<tofd<<":"<<name<<":"<<message<<endl;
			_str = "send success!";
			if(-1 == send(tofd,message,strlen(message),0))
			{
				cerr<<"send (talkone1)response fail;errno:"<<errno<<endl;
				return ;
			}
		}
		else//不在线则将消息加入offline表中
		{
			char cmd2[100] = "INSERT INTO offline VALUE('"; 
			sprintf(cmd2+strlen(cmd2),"%s",name);
			sprintf(cmd2+strlen(cmd2),"%s","','");
			sprintf(cmd2+strlen(cmd2),"%s",message);
			sprintf(cmd2+strlen(cmd2),"%s","');");

			if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
			{
				cerr << "select fail;errno:"<<errno<<endl;
				return ;
			}
		}
	}
	else
	{
		cout<<"not found"<<endl;
		_str = "send fail!";
		
	}
}

void View_TalkOne::response()
{
	char buff[128] = {0};
	strcpy(buff,_str);
	cout<<_str<<endl;
	if(-1 == send(_fd,buff,strlen(buff),0))
	{
		cerr<<"send (talkone)response fail;errno:"<<errno<<endl;
		return ;
	}

}
