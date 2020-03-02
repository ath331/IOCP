#include "DB.h"
#include <iostream>

using namespace std;

void DB::Init()
{
	mysql_init(&_mysql);
	if (!mysql_real_connect(&_mysql, "localhost","root", "xoghks105", "iocp_chat_db", 3306, NULL, 0))
	{
		cout << "DB connect error" << endl;
	}
}

void DB::InsertData()
{

}

void DB::UpdateData()
{

}

void DB::CloseDB()
{
	mysql_close(&_mysql);
}
