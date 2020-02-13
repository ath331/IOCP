#include "Server/Server.h"
#include "Interface.h"

int main()
{
	Interface i;
	i.InterfaceMake();

	Server server;
	server.InputPortNum();
	server.InitServer();
	server.RunServer();

	return 0;
}