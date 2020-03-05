#pragma once
#pragma comment (lib,"libmySQL.lib")
#include <mysql.h>

class DB
{
public:
	void Init();
	void InsertData();
	void UpdateData();
	void CloseDB();
private:
	MYSQL _mysql;
};