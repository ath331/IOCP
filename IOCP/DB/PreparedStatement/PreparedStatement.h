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
		_prep_stmt = con->prepareStatement(str.c_str());
	}

	~PreparedStatement()
	{
		delete _prep_stmt;
	}

	void SetSqlStr(vector<string>& strVec);
	bool Execute();

private:
	sql::PreparedStatement* _prep_stmt;
};