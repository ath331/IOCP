#pragma once
#pragma comment (lib,"libmySQL.lib")
#include <iostream>

using namespace std;

class DB
{
public:
	bool InsertData(string id, string pw, string name);
	bool CheckIdPw(string id, string pw);
	string GetName(string id);
	void UpdateData();
	void CloseDB();

	DB();
	~DB();

private:
	struct _MysqlStruct;
	unique_ptr<_MysqlStruct> _mysqlInstance;
};
