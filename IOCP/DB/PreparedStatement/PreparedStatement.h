#pragma once
#include <string>
#include <vector>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

using namespace std;

class PreparedStatement
{
public:
	PreparedStatement(sql::Connection* con, string str)
	{
		_preStmt = con->prepareStatement(str.c_str());
	}

	~PreparedStatement()
	{
		delete _preStmt;
	}

	void SetSqlStr(vector<string>& strVec);
	bool ExecuteUpdate();

private:
	sql::PreparedStatement* _preStmt;
};