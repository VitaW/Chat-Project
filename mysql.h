#ifndef MYSQL_H
#define MYSQL_H
#include<mysql/mysql.h>
#include <errno.h>
#include <iostream>
#include <json/json.h>
#include "register.h"
#include "login.h"
#include "exit.h"

using namespace std;

class Mysql
{
public:
	Mysql();
	~Mysql();
//private:
	MYSQL* _mpcon;
	MYSQL_RES* _mp_res;
	MYSQL_ROW _mp_row;
	friend class View_register;
	friend class View_login;
	friend class View_exit;
};
#endif
