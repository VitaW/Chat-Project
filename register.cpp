#include"register.h"
#include"mysql.h"
#include <json/json.h>

extern Mysql Mysql_sever;

void View_register::process(int fd,char* json)
{
	cout<<"View_register:: process run"<<endl;
	_fd = fd;
	//解析 json
	//name    pw
	Json::Value val;
	Json::Reader read;
        if(-1 == read.parse(json,val))
	{
		cerr<<"read fail;errno:"<<errno<<endl;
		return ;
	}
	

        const char* name = val["name"].asString().c_str();

 	char name1[128] = {0};
 	sprintf(name1,"%s",name);

	//在数据库中查找name有没有重复
	char cmd[100] = "SELECT * FROM user WHERE name='";
	sprintf(cmd+strlen(cmd),"%s",name);
	sprintf(cmd+strlen(cmd),"%s","';");

	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
	   cerr<<"select one fail(register);errno:"<<errno<<endl;
	   return ;
	}

	const char* pw = val["pw"].asString().c_str();

   	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
	if(!(Mysql_sever._mp_row = mysql_fetch_row(Mysql_sever._mp_res)))   //name没有重复的
	{
		//将name pw 加入到数据库的user表
		char cmd2[100] = "INSERT INTO user VALUE('";
		sprintf(cmd2+strlen(cmd2),"%s",name1);
		sprintf(cmd2+strlen(cmd2),"%s","','");
		sprintf(cmd2+strlen(cmd2),"%s",pw);
		sprintf(cmd2+strlen(cmd2),"%s","');");

       	if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
	   	{
	       		cerr<<"insert fail;errno:"<<errno<<endl;
		       return ;
	   	}
        
		_str = "register sucess!";
	
	}
	else
	{
		_str = "register fail!";
	}
  
}

void View_register::response()
{	
	char buff[128] = {0};
	strcpy(buff,_str);
	cout<<_str<<endl;
	if(-1 == send(_fd,buff,strlen(buff),0))
	{
		cerr<<"send (register)response fail;errno:"<<errno<<endl;
		return ;
	}
	
}
