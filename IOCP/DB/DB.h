#pragma once
#pragma comment (lib,"libmySQL.lib")
#include <iostream>

using namespace std;

class DB
{
public:
	void InsertData();
	void UpdateData();
	void CloseDB();

	DB();
	~DB();

private:
	struct _MysqlStruct;
	unique_ptr<_MysqlStruct> _mysqlInstance;
};
