#include "tcpsever.h"

void listen_cb(int fd,short event,void *arg)
{
	Tcpsever *mythis = (Tcpsever*)arg;
	//接受用户链接
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int cli_fd = accept(fd,(struct sockaddr*)&caddr,&len);
	if(-1 == cli_fd)
	{
	   cerr<<"accepe fail;errno:"<<errno<<endl;
	   throw " ";
	}
        //查找当前监听数量最少的子线程
	map<int,int>::iterator it = mythis->_pth_work_num.begin();
	int second_less = it->second;
	int fd_less = it->first;;
	while(it != mythis->_pth_work_num.end())
	{
	   if(it->second < second_less)
	   {
	      	second_less = it->second;
            fd_less = it->first;
	   }
	   it++;
	}
    
	char buff[100] = {0};
	sprintf(buff,"%d",cli_fd);
    //将客户端套接子通过socktpair发给子线程
	if(-1 == send(fd_less,buff,strlen(buff),0))
	{
	  cerr<<"send cli_fd fail;errno:"<<errno<<endl;
      return ;
	}
	
}

void sock_pair_cb(int fd,short event,void *arg)
{
	//读取管道内容
	Tcpsever *mythis = (Tcpsever*)arg;
	 char buff[100] = {0};
	 if( 0 > recv(fd,buff,99,0))
	 {
	   cerr<<"recv fail;errno:"<<errno<<endl;
	   return ;
	 }
    int tmp = atoi(buff);
  
	//更新到map表_pth_work_num  ----->fd
	mythis->_pth_work_num[fd] = tmp;
}


Tcpsever::Tcpsever(char *ip,short port,int pth_num)
{
	///创建服务器
	_listen_fd = socket(AF_INET,SOCK_STREAM,0);
	 if(-1 == _listen_fd)
	 {
	    cerr<<"fd creat fail;errno:"<<errno<<endl;
		throw " ";
	 }
    struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	if(-1 == bind(_listen_fd,(struct sockaddr*)&saddr,sizeof(saddr)))
	{
	   cerr<<"bind fail;errno:"<<errno<<endl;
	   throw " ";
	}

    if(-1 == listen(_listen_fd,20))
	{
	  cerr<<"listen fail;errno:"<<errno<<endl;
	  throw " ";
	}

	_pth_num = pth_num;

	//给libevent申请空间
	_base = event_base_new();

	//创建事件，绑定监听套接子的回调函数(listen_cb)
   struct event* listen_event = event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,this);
   if(NULL == listen_event)
   {
      cerr<<"event new fail;errno:"<<errno<<endl;
	throw " ";
   }
   event_add(listen_event,NULL);

}
Tcpsever::~Tcpsever()
{
  
}

void Tcpsever::get_sock_pair()
{
	for(int i = 0;i < _pth_num;i++ )
	{
		//申请双向管道
		int arr[2];
            if(socketpair(PF_LOCAL,SOCK_STREAM,0,arr) < 0)
		{
		   cerr<<"socketpair creat fail;errno:"<<endl;
		   return ;
		}
		//将双向管道加入到_sock_pair.push_back();
		_socket_pair.push_back(vec(arr));

		_pth_work_num.insert(make_pair(arr[0], 0));
	}
}

void Tcpsever::get_pthread()
{
	//开辟线程
	for(int i = 0; i< _pth_num; i++)
	{
       //		_pthread.push_bach(new Pthread(_sock_pair[i][1]));
	   	_pthread.push_back(new Pthread(_socket_pair[i].arr[1]));
	}
}

void Tcpsever::run()
{
	//申请socketpair（函数自查）
	get_sock_pair();
	//创建线程
	get_pthread();
	//规定  int arr[2]  arr[0]<=>主线程占用   arr[1]<=>子线程占用
	//为主线程的socktpair创建事件，绑定回调函数（sock_pair_cb）
	int i = 0;
	for(;i<_pth_num;i++)
	{	   
	      struct event* socketpair_event = event_new(_base,_socket_pair[i].arr[0],EV_READ|EV_PERSIST,sock_pair_cb,this);
		if(NULL == socketpair_event)
		{
			cerr<<"socketpair_event fail;errno:"<<errno<<endl;
			return ;
		}
	      event_add(socketpair_event,NULL);
	}
      event_base_dispatch(_base);
}


