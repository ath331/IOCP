#include "Server/Server.h"

#pragma comment (lib,"WS2_32.lib")

int main()
{
	Server server;
	//server.InputPortNum();
	server.RunServer();

	return 0;
}