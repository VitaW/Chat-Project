#include "exit.h"
#include "mysql.h"

extern Mysql Mysql_sever;

//主要做的事情：把online 中的数据删除
void View_exit::process(int fd,char* json)
{
	_fd = fd;
	
	Json::Value val;
	Json::Reader read;
	
	if(-1 == read.parse(json,val))
	{
		cerr <<"json parse fail;errno:"<<errno<<endl;
		return ;
	}
	
	const char *getname = val["name"].asString().c_str();
	char name1[100] = {0};
	sprintf(name1,"%s",getname);
	char cmd[100] = "SELECT * FROM online WHERE name='";
   	sprintf(cmd+strlen(cmd),"%s",getname);
	sprintf(cmd+strlen(cmd),"%s","';");

	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cerr << "select fail(exit);errno:"<<errno<<endl;
		return ;
	}
	
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
	//若没找到该用户 
	if(!(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res)))
	{
		_str = "exit fail!";
	}
	else //找到且有消息，则插入offline表中 
	{
		char buff[128] = "DELETE FROM online WHERE name='";
		sprintf(buff+strlen(buff),"%s",name1);
		sprintf(buff+strlen(buff),"%s","';");

		if(mysql_real_query(Mysql_sever._mpcon,buff,strlen(buff)))
		{
			cerr<<"delete fail;errno:"<<errno<<endl;
			_str = "delete fail";
			 return ;
		}
		
		_str= "exit success!";   
 	}
}

void View_exit::response()
{
	char buff[128] = {0};
	strcpy(buff,_str);
	cout<<_str<<endl;
	if(-1 == send(_fd,buff,strlen(buff),0))
	{
		cerr<<"send (exit)response fail;errno:"<<errno<<endl;
		return ;
	} 
}
