#pragma once
#include <string>
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

	void SetString(int index, string value);
	bool Execute();

private:
	sql::PreparedStatement* _prep_stmt;
};