#pragma once
#pragma comment (lib,"libmySQL.lib")
#pragma comment (lib,"mysqlcppconn.lib")

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
	void UpdateData(UpdataType type, string id = "tempId", string name = "tempName", int sock = -1);
	void CloseDB();

	DB();
	~DB();

private:
	struct _MysqlStruct;
	unique_ptr<_MysqlStruct> _mysqlInstance;

	const char* _dbIP = "tcp://127.0.0.1:3306";
};
