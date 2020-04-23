#include "DB.h"
#include <stdio.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include "ResultSet/ResultSet.h"
#include "PreparedStatement/PreparedStatement.h"

struct DB::_MysqlStruct
{
	sql::Connection* con;
	sql::mysql::MySQL_Driver* driver;
};

DB::DB() : _mysqlInstance(make_unique<_MysqlStruct>())
{
	_mysqlInstance->driver = sql::mysql::get_mysql_driver_instance();
	_mysqlInstance->con = _mysqlInstance->driver->connect(_dbIP, _name, _pw);
	cout << "DB IP : " << _dbIP << endl;
	_mysqlInstance->con->setSchema("iocp_chat_db");

	UpdateData(UpdataType::SOCK);
}

DB::~DB() {}

bool DB::InsertData(string id, string pw, string name)
{
	PreparedStatement prepareStatement(_mysqlInstance->con, "INSERT INTO clientinfo VALUES (?, ?, ?, ?)");
	vector<string> sqlVec = { id,pw,name,"-1" };
	prepareStatement.SetSqlStr(sqlVec);

	return prepareStatement.ExecuteUpdate();
}

bool DB::CheckIdPw(string id, string pw)
{
	ResultSet resultSet(_mysqlInstance->con);
	return resultSet.CheckIdPw(id, pw);
}

string DB::GetName(string id)
{
	ResultSet resultSet(_mysqlInstance->con);
	return resultSet.GetName(id);
}

void DB::UpdateData(UpdataType type, string id, string name, int sock)
{
	switch (type)
	{
	case UpdataType::NAME:
	{
		PreparedStatement prepareStatement(_mysqlInstance->con, "UPDATE clientInfo SET clientName = ? WHERE clientSock = ?");
		vector<string> sqlVec = { name,to_string(sock) };
		prepareStatement.SetSqlStr(sqlVec);

		prepareStatement.ExecuteUpdate();
	}
	break;

	case UpdataType::SOCK:
	{
		if (id == "tempId")
		{
			PreparedStatement prepareStatement(_mysqlInstance->con, "UPDATE clientInfo SET clientSock = ?");
			vector<string> sqlVec = { to_string(sock) };
			prepareStatement.SetSqlStr(sqlVec);

			prepareStatement.ExecuteUpdate();
		}

		else if (id != "tempId")
		{
			PreparedStatement prepareStatement(_mysqlInstance->con, "UPDATE clientInfo SET clientSock = ? where clientID = ?");
			vector<string> sqlVec = { to_string(sock),id };
			prepareStatement.SetSqlStr(sqlVec);

			prepareStatement.ExecuteUpdate();
		}
	}
	break;
	default:
		break;
	}
}

void DB::CloseDB()
{
	delete _mysqlInstance->con;
}
