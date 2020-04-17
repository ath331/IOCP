#pragma once
#include <string>
#include <cppconn/resultset.h>
#include "../PreparedStatement/PreparedStatement.h"

using namespace std;

class ResultSet : public PreparedStatement
{
public:
	ResultSet(sql::Connection* con, string str)
		: PreparedStatement(con, str)
	{
		try
		{
			_res = _preStmt->executeQuery();

		}
		catch (sql::SQLException & e)
		{
			cout << "SQLException error" << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode() << " )" << endl;
		}
	}

	~ResultSet()
	{
		delete _res;
	}

	bool CheckIdPw(string id, string pw);
	string GetName(string id);
private:
	sql::ResultSet* _res;
};