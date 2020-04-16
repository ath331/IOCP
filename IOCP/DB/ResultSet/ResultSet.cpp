#include "ResultSet.h"

bool ResultSet::CheckIdPw(string id, string pw)
{
	while (_res->next())
	{
		if (strcmp(_res->getString("clientID").c_str(),id.c_str()) == 0)
			if (strcmp(_res->getString("clientPW").c_str(),pw.c_str()) == 0)
				return true;
	}
	return false;
}

string ResultSet::GetName(string id)
{
	while (_res->next())
	{
		if (strcmp(_res->getString("clientID").c_str(), id.c_str()) == 0)
			return static_cast<string>(_res->getString("clientName").c_str());
	}

	return static_cast<string>("DEFUALT");
}
