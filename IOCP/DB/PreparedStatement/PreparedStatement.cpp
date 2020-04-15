#include "PreparedStatement.h"

void PreparedStatement::SetString(int index, string value)
{
	_prep_stmt->setString(index, value.c_str());
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
