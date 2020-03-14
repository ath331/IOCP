#include "Server/Server.h"

int main()
{
	Server server;
	//server.InputPortNum();
	server.InitServer();
	server.RunServer();

	return 0;
}