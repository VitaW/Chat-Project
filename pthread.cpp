#include"pthread.h"
#include "control.h"
#include <json/json.h>
extern Control control_sever;

void client_cb(int fd,short event,void *arg)
{
	//recv  ->buff
	Pthread* mythis = (Pthread*)arg;

	char buff[256]= {0};
	if(0 > recv(fd,buff,255,0))
	{
		cerr<<"recv cli_fd fail;errno:"<<errno<<endl;
	   return ;
	}

      char mybuff[256] = {0};
	sprintf(mybuff,"%s",buff);
	 
      Json::Value val;
	Json::Reader read;
	if(-1 == read.parse(buff,val))
	{
		cerr<<"read fail;"<<endl;
		return ;
	}

      if( val["reason_type"].asInt() == MSG_TYPE_EXIT)//客户端发送过来的是exit,先把map表中的事件删除，再将online中的数据进行删除（让视图去做）
	{
		map<int,struct event*>::iterator it = mythis->_event_map.find(fd);
		mythis->_event_map.erase(it);
		event_del(it->second);
	}

	//将buff发给control
	control_sever.process(fd,mybuff);
}
void sock_pair_1_cb(int fd,short event,void *arg)
{
    Pthread* mythis = (Pthread*)arg;
	//recv -> clien_fd
	char buff[128] = {0};
	if(0 > recv(fd,buff,127,0) )
	{
	  cerr<<"pthread recv cli_fd fail;errno:"<<errno<<endl;
	  return ;
	}
	int cli_fd = atoi(buff);
	//将client_fd加入到libevent     client_cb()
	struct event* listen_event =  event_new(mythis->_base,cli_fd,EV_READ|EV_PERSIST,client_cb,mythis);
      event_add(listen_event,NULL);

      mythis->_event_map.insert(make_pair(cli_fd,listen_event));	//用于exit时删除事件用。

	//给主线程回复当前监听的客户端数量
	
	int num = mythis->_event_map.size();
	char sbuff[100] = {0};
	sprintf(sbuff,"%d",num);
      if(-1 == send(fd,sbuff,strlen(sbuff),0))
	{
	   cerr<<"send fail;errno:"<<errno<<endl;
       return ;
	}

}
void* pthread_run(void *arg)
{
	//将sock_pair_1加入到libevent  sock_pair_1_cb()
	Pthread* mythis = (Pthread*)arg;
	struct event* listen_event = event_new(mythis->_base,mythis->_sock_fd,EV_READ|EV_PERSIST,sock_pair_1_cb,mythis);
	if(NULL == listen_event)
	{
	  cerr<<"event new fail;errno:"<<errno<<endl;
	}
   event_add(listen_event,NULL);

   event_base_dispatch(mythis->_base);
}


Pthread::Pthread(int sock_fd)
{
	_sock_fd = sock_fd;
      _base = event_base_new();

	//启动线程
	_pthread = pthread_create(&_pthread,NULL,pthread_run,(void*)this);
}


Pthread::~Pthread()
{

}

