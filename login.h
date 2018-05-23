#ifndef LOGIN
#define LOGIN
#include "View.h"
#include <errno.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

class View_login :public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
    const char* _str;
};
#endif


