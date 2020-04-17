#include "PreparedStatement.h"
#include "Log.h"

void PreparedStatement::SetSqlStr(vector<string>& strVec) const
{
	for (int i = 0; i < strVec.size(); i++)
	{
		_preStmt->setString(i + 1, strVec[i].c_str());
	}
}

bool PreparedStatement::ExecuteUpdate() const
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
		Log log(LogIndex::WARNING, e.what(), e.getErrorCode());
		return false;
	}

	return true;
}

