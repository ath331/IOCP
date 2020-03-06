#include "DB.h"
#include <mysql.h>

struct DB::_MysqlStruct
{
	MYSQL _conn;
	MYSQL* _connPtr = NULL;
	MYSQL_RES* _result;
	MYSQL_ROW _row;
};

DB::DB() : _mysqlInstance(make_unique<_MysqlStruct>())
{
	mysql_init(&(_mysqlInstance->_conn));
	_mysqlInstance->_connPtr =
		mysql_real_connect(&(_mysqlInstance->_conn), "localhost", "root", "xoghks105", "iocp_chat_db", 3306, NULL, 0);
	if (_mysqlInstance->_connPtr == NULL)
		cout << "DB::DB() Error" << endl;
}

DB::~DB()
{

}

void DB::InsertData()
{

}

void DB::UpdateData()
{

}

void DB::CloseDB()
{
	mysql_close(_mysqlInstance->_connPtr);
}
