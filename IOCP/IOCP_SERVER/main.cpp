#include "Server/Server.h"

int main()
{
	Server server;
	//server.InputPortNum();
	server.InitServer();
	server.RunServer();
	server.CloseServer();

	return 0;
}