#ifndef GETLIST_H
#define GETLIST_H
#include"View.h"
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <string.h>
#include  <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

class View_GetList : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	const char*  _str;
};
#endif
