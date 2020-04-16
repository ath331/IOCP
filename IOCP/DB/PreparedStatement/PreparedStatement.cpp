#include "PreparedStatement.h"

void PreparedStatement::SetSqlStr(vector<string>& strVec)
{
	for (int i = 0; i < strVec.size(); i++)
	{
		_preStmt->setString(i + 1, strVec[i].c_str());
	}
}

bool PreparedStatement::ExecuteUpdate()
{
	try
	{
		if (_preStmt->executeUpdate() < 0)
		{
			throw;
		}
	}
	catch (sql::SQLException & e)
	{
		cout << "SQLException error" << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode() << " )" << endl;
		return false;
	}

	return true;
}

