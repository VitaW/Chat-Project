#ifndef EXIT_H
#define EXIT_H
#include "View.h"
#include <string>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <json/json.h>
#include <stdio.h>
using namespace std;

class View_exit : public View
{
public:
	void process(int fd,char* json);
	void response();
private:
	int _fd;
	const char* _str;

};

#endif
