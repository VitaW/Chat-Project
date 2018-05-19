#include "mysql.h"
#include "login.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <json/json.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <iostream>
using namespace std;
extern Mysql Mysql_sever;

char name2[128] = {0};
void View_login::process(int fd,char* json)
{
	cout<<"View_login:: process run"<<endl;
	//解析 json
	//name    pw
	_fd = fd;
	Json::Value root;
	Json::Reader read;
	if(-1 == read.parse(json,root))
	{
		cerr<<"login json prase fail;errno:"<<errno<<endl;
		return ;
	}
	const char* name = root["name"].asString().c_str();
	
	char name1[128] = {0};
	sprintf(name1,"%s",name);
	sprintf(name2,"%s",name);
	//在数据库中查找name有没有重复
	char cmd2[100] = "SELECT * FROM user WHERE name='";
	sprintf(cmd2+strlen(cmd2), "%s", name);
	sprintf(cmd2+strlen(cmd2), "%s", "';");

	if(mysql_real_query(Mysql_sever._mpcon, cmd2, strlen(cmd2)))
	{
		cerr<<"mysql_real_query fail;errno:"<<errno<<endl;
		return;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
	if(!(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))) //没找到
	{
	   _str = "name not find!";
	}
	const char* pw = root["pw"].asString().c_str();

    if(strcmp(pw,Mysql_sever._mp_row[1]) == 0)//账户存在且密码相等
	{//账户密码是否正确	
		char cmd[100] = "SELECT * FROM online WHERE name='";
		sprintf(cmd+strlen(cmd), "%s",name1);
		sprintf(cmd+strlen(cmd), "%s","';");

		if(mysql_real_query(Mysql_sever._mpcon, cmd, strlen(cmd)))
		{
			cerr<<"mysql_real_query fail;errno:"<<errno<<endl;
			return;
		}
		Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
		
		if(!(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res)))
		{//是否在线
			char cmd1[100] = "INSERT INTO online VALUE(";
			sprintf(cmd1+strlen(cmd1), "%d",fd);
			sprintf(cmd1+strlen(cmd1), "%s",",'");
			sprintf(cmd1+strlen(cmd1), "%s",name1);
			sprintf(cmd1+strlen(cmd1), "%s","');");
			
			if(mysql_real_query(Mysql_sever._mpcon,cmd1,strlen(cmd1)))
			{
				cerr<<"insert online fail;errno:"<<errno<<endl;
				return;
			}
			_str = "login success!";
///////////////////////
			
////////////////////
		}
		else
		{
			_str= "login fail!";
		}
	}
	else
	{
		_str = "pw errno!";
	}

}

void View_login::response()
{	
	char buff[128] = {0};
	strcpy(buff,_str);
	cout<<_str<<endl;
	if(-1 == send(_fd,buff,strlen(buff),0))
	{
		cerr<<"send (login)response fail;errno:"<<errno<<endl;
		return ;
	}

	char cmd3[100] = "SELECT * FROM offline WHERE name='";
	sprintf(cmd3+strlen(cmd3), "%s",name2);
	sprintf(cmd3+strlen(cmd3), "%s","';");

	if(mysql_real_query(Mysql_sever._mpcon, cmd3, strlen(cmd3)))
	{
		cerr<<"mysql_real_query fail;errno:"<<errno<<endl;
		return;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);		
	if(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res))
	{	
		char mess[128] = {0};
		strcpy(mess,Mysql_sever._mp_row[1]);
		if(-1==send(_fd,mess,strlen(mess),0))
		{
			cerr<<"send (mess)response fail;errno:"<<errno<<endl;
			return ;
		}
	}
}
