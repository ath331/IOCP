#include "PreparedStatement.h"

void PreparedStatement::SetSqlStr(vector<string>& strVec)
{
	for (int i=0 ; i < strVec.size() ; i++)
	{
		_prep_stmt->setString(i+1, strVec[i].c_str());
	}
}

bool PreparedStatement::Execute()
{
	try
	{
		_prep_stmt->execute();
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
