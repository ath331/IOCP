#include "Server/Server.h"
#include "Interface.h"

int main()
{
	Server server;
	server.InputPortNum();
	server.InitServer();
	server.RunServer();

	return 0;
}