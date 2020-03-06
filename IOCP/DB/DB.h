#pragma once
#pragma comment (lib,"libmySQL.lib")
#include <iostream>

using namespace std;

class DB
{
public:
	void InsertData(string id, string pw, string name);
	void UpdateData();
	void CloseDB();

	DB();
	~DB();

private:
	struct _MysqlStruct;
	unique_ptr<_MysqlStruct> _mysqlInstance;
};
