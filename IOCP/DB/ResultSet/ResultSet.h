#pragma once
#include <string>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

using namespace std;

//TODO : PreparedStatement clss 상속받기
class ResultSet
{
public:
	ResultSet(sql::Connection* con, string str)
	{
		try
		{
			_stmt = con->prepareStatement(str.c_str());
			_res = _stmt->executeQuery();

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
		delete _stmt;
	}

	bool CheckIdPw(string id, string pw);
	string GetName(string id);
private:
	sql::ResultSet* _res;
	sql::PreparedStatement* _stmt;
};