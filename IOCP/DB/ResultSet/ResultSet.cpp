#include "ResultSet.h"
#include "../../IOCP_SERVER/Basic/Log/Log.h"

bool ResultSet::CheckIdPw(string id, string pw) const
{
	while (_res->next())
	{
		if (strcmp(_res->getString("clientID").c_str(),id.c_str()) == 0)
			if (strcmp(_res->getString("clientPW").c_str(),pw.c_str()) == 0)
				return true;
	}
	return false;
}

string ResultSet::GetName(string id) const
{
	while (_res->next())
	{
		if (strcmp(_res->getString("clientID").c_str(), id.c_str()) == 0)
			return static_cast<string>(_res->getString("clientName").c_str());
	}

	return static_cast<string>("DEFUALT");
}

void ResultSet::_ExecuteQuery()
{
	try
	{
		_res = _preStmt->executeQuery();

	}
	catch (sql::SQLException & e)
	{
		Log log(LogIndex::WARNING, e.what(), e.getErrorCode());
	}
}

