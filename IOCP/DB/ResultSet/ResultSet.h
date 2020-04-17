#pragma once
#include <string>
#include <cppconn/resultset.h>
#include "../PreparedStatement/PreparedStatement.h"

using namespace std;

class ResultSet : public PreparedStatement
{
public:
	ResultSet(sql::Connection* con)
		: PreparedStatement(con, "SELECT * FROM clientinfo")
	{
		_ExecuteQuery();
	}

	ResultSet(sql::Connection* con, string str)
		: PreparedStatement(con, str)
	{
		_ExecuteQuery();
	}

	~ResultSet()
	{
		delete _res;
	}

	bool CheckIdPw(string id, string pw);
	string GetName(string id);
private:
	sql::ResultSet* _res;
	void _ExecuteQuery();
};