#include "getlist.h"
#include "mysql.h"

extern Mysql Mysql_sever;

void View_GetList::process(int fd,char *json)
{
	cout<<"View_get_list:process run"<<endl;
	_fd = fd;
	char list[512] = {0};

	Json::Value val;
	Json::Reader read;
	if(-1 == read.parse(json,val))
	{
		cerr <<"json parse fail;errno:"<<errno<<endl;
		return ;
	}
	
	char cmd[512] = "SELECT name FROM online;";
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
	   cerr<<"select one fail(getlist1);errno:"<<errno<<endl;
	   return ;
	}
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon); 
	Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res);
	strcpy(list,Mysql_sever._mp_row[0]);
	sprintf(list+strlen(list),"%s"," ");
	while(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res)) //有名字则读取
	{
		char *tmp = Mysql_sever._mp_row[0];
		sprintf(list+strlen(list),"%s",tmp);
		strcat(list," ");
	}
	_str = list;

}

void View_GetList::response()
{
	char buff[128] = {0};
	strcpy(buff,_str);
	cout<<_str<<endl;
	if(-1 == send(_fd,buff,strlen(buff),0))
	{
		cerr<<"send (getlist)response fail;errno:"<<errno<<endl;
		return ;
	}
}
