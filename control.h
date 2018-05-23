#ifndef CONTROL_H
#define CONTROL_H
#include "View.h"
#include <string>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <json/json.h>
#include <stdlib.h>
#include "public.h"
#include <map>
using namespace std;

class Control
{
public:
	Control();
	~Control();
	void process(int fd,char* json);
private:
	map<int,View*> _map;
};
#endif
