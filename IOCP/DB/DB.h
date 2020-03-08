#pragma once
#pragma comment (lib,"libmySQL.lib")
#include <iostream>

using namespace std;

enum class UpdataType : int
{
	NAME,
	SOCK,
};

class DB
{
public:
	bool InsertData(string id, string pw, string name);
	bool CheckIdPw(string id, string pw);
	string GetName(string id);
	void UpdateData(UpdataType type, string id, string name, int sock = -1);
	void SelectDBTable(bool isResetResult = false);
	void CloseDB();

	DB();
	~DB();

private:
	struct _MysqlStruct;
	unique_ptr<_MysqlStruct> _mysqlInstance;
};
