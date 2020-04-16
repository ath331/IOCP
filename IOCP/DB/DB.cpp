#include "DB.h"
#include <mysql.h>
#include <string>
#include <stdio.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include "PreparedStatement/PreparedStatement.h"

struct DB::_MysqlStruct
{
	MYSQL _conn;
	MYSQL* _connPtr = NULL;
	MYSQL_RES* _result;
	MYSQL_ROW _row;

	sql::Connection* con;
	sql::mysql::MySQL_Driver* driver;
};

DB::DB() : _mysqlInstance(make_unique<_MysqlStruct>())
{
	mysql_init(&(_mysqlInstance->_conn));
	_mysqlInstance->_connPtr =
		mysql_real_connect(&(_mysqlInstance->_conn), "localhost", "root", "xoghks105", "iocp_chat_db", 3306, NULL, 0);
	if (_mysqlInstance->_connPtr == NULL)
		cout << "DB::DB() Error" << endl;

	UpdateData(UpdataType::SOCK);

	_mysqlInstance->driver = sql::mysql::get_mysql_driver_instance();
	_mysqlInstance->con = _mysqlInstance->driver->connect("tcp://127.0.0.1:3306", "root", "xoghks105");
	_mysqlInstance->con->setSchema("iocp_chat_db");
}

DB::~DB() {}

bool DB::InsertData(string id, string pw, string name)
{
	PreparedStatement prepareStatement(_mysqlInstance->con, "INSERT INTO clientinfo VALUES (?, ?, ?, ?)");
	vector<string> strVec = { id,pw,name,"-1" };
	prepareStatement.SetSqlStr(strVec);

	return prepareStatement.Execute();
}

bool DB::CheckIdPw(string id, string pw)
{
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
	SelectDBTable();
	while ((_mysqlInstance->_row = mysql_fetch_row(_mysqlInstance->_result)) != NULL)
	{
		if (_mysqlInstance->_row[0] == id)
		{
			SelectDBTable(TRUE);
			return (string)_mysqlInstance->_row[2];
		}
	}

	return static_cast<string>("DEFUALT");
}

void DB::UpdateData(UpdataType type, string id, string name, int sock)
{
	SelectDBTable();
	switch (type)
	{
	case UpdataType::NAME:
	{
		string updateName = "UPDATE clientInfo set clientName = '";
		updateName += name;
		updateName += "' WHERE clientSock ='";
		updateName += to_string(sock);
		updateName += "'";

		int result = mysql_query(_mysqlInstance->_connPtr, updateName.c_str());
		if (result != 0)
			cout << "mysql_query(_mysqlInstance->_connPtr, updateName.c_str() error" << endl;

	}
	break;

	case UpdataType::SOCK:
	{
		string updateSock = "UPDATE clientInfo set clientSock = '";
		updateSock += to_string(sock);
		updateSock += "'";

		if (id == "tempId")
		{
			int result = mysql_query(_mysqlInstance->_connPtr, updateSock.c_str());
			if (result != 0)
				cout << "mysql_query(_mysqlInstance->_connPtr, updateSock.c_str()  sock = -1 error" << endl;
		}

		else if (id != "tempId")
		{
			updateSock += "where clientID = '";
			updateSock += id;
			updateSock += "'";
			int result = mysql_query(_mysqlInstance->_connPtr, updateSock.c_str());
			if (result != 0)
				cout << "mysql_query(_mysqlInstance->_connPtr, updateSock.c_str() error" << endl;
		}
	}
	break;
	default:
		break;
	}
	SelectDBTable(TRUE);
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
	delete _mysqlInstance->con;
}
