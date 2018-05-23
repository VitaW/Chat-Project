#ifndef REGISTER
#define REGISTER
#include"View.h"
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include  <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
using namespace std;

class View_register : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	const char*  _str;
};

#endif
