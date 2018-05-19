#include"control.h"
#include "register.h"
#include "login.h"
#include "talkone.h"
#include "talkgroup.h"
#include "getlist.h"
#include "exit.h"
Control::Control()
{
	_map.insert(make_pair(MSG_TYPE_REGISTER,new View_register));
	_map.insert(make_pair(MSG_TYPE_LOGIN,new View_login));
	_map.insert(make_pair(MSG_TYPE_TALK_TO_ONE,new View_TalkOne));
	_map.insert(make_pair(MSG_TYPE_TALK_TO_GROUP,new View_TalkGroup));
	_map.insert(make_pair(MSG_TYPE_GET_LIST,new View_GetList));
	_map.insert(make_pair(MSG_TYPE_EXIT,new View_exit));
}

void Control::process(int fd,char* json)
{
	char flg[256] = {0};
	strcpy(flg,json);
	//解析json，获取消息类型
	Json::Value val;
	Json::Reader read;
	if(-1 == read.parse(json,val))
	{
	   cerr<<"read fail;errno:"<<errno<<endl;
	}
	//根据消息类型在map中查找
  	map<int,View*>::iterator it = _map.find(val["reason_type"].asInt());

	//判断是否找到
	if(it == _map.end())
	{
	  cerr<<"no find MSG::type;"<<endl;
	  return ;
	}
	
	it->second->process(fd,flg);
	it->second->response();
	return ;
}


Control::~Control()
{
   
}


Control control_sever;
