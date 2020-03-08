#include "DB.h"
#include <mysql.h>
#include <string>
#include <stdio.h>
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

	UpdateData(UpdataType::SOCK,NULL,NULL,NULL);
}

DB::~DB() {}

bool DB::InsertData(string id, string pw, string name)
{
	int rowCount = 0;
	SelectDBTable(TRUE);
	SelectDBTable();
	while ((_mysqlInstance->_row = mysql_fetch_row(_mysqlInstance->_result)) != NULL)
	{
		rowCount++;
	}
	SelectDBTable(TRUE);

	string insertQuery = "INSERT INTO clientinfo values('";
	insertQuery += to_string(rowCount);
	insertQuery += "','";
	insertQuery += id;
	insertQuery += "','";
	insertQuery += pw;
	insertQuery += "','";
	insertQuery += name;
	insertQuery += "')";

	int result = mysql_query(_mysqlInstance->_connPtr, insertQuery.c_str());
	if (result != 0)
	{
		cout << "mysql_query(_mysqlInstance->_connPtr,insertQuery.c_str()) error" << endl;
		fprintf(stderr, "%s\n", mysql_error(&(_mysqlInstance->_conn)));
		return FALSE;
	}
	return TRUE;
}

bool DB::CheckIdPw(string id, string pw)
{
	SelectDBTable(TRUE);
	SelectDBTable();
	while ((_mysqlInstance->_row = mysql_fetch_row(_mysqlInstance->_result)) != NULL)
	{
		if (_mysqlInstance->_row[0] == id)
			if (_mysqlInstance->_row[1] == pw)
			{
				SelectDBTable(TRUE);
				return TRUE;
			}
	}
	return FALSE;
}

string DB::GetName(string id)
{
	SelectDBTable(TRUE);
	SelectDBTable();
	while ((_mysqlInstance->_row = mysql_fetch_row(_mysqlInstance->_result)) != NULL)
	{
		if (_mysqlInstance->_row[0] == id)
			return (string)_mysqlInstance->_row[2];
	}
}

void DB::UpdateData(UpdataType type, string id, string name, int sock)
{
	switch (type)
	{
	case UpdataType::NAME:
		break;
	case UpdataType::SOCK:
	{
		SelectDBTable(TRUE);
		SelectDBTable();
		while ((_mysqlInstance->_row = mysql_fetch_row(_mysqlInstance->_result)) != NULL)
		{
			_mysqlInstance->_row[3] = (char*)-1;
		}
	}
		break;
	default:
		break;
	}
}

void DB::SelectDBTable(bool isResetResult)
{
	if (isResetResult)
	{
		mysql_free_result(_mysqlInstance->_result);
	}

	else
	{
		string selectQuery = "SELECT* FROM clientInfo";
		int result = mysql_query(_mysqlInstance->_connPtr, selectQuery.c_str());
		if (result != 0)
			cout << "mysql_query(_mysqlInstance->_connPtr, selectQuery.c_str()) error <<endl;" << endl;
		_mysqlInstance->_result = mysql_store_result(_mysqlInstance->_connPtr);
	}
}

void DB::CloseDB()
{
	mysql_close(_mysqlInstance->_connPtr);
}
