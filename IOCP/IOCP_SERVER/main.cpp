#include<iostream>

#include "Server/Server.h"

void ErrorHandling(const char* msg);

int main()
{
	Server server;
	server.InputPortNum();
	server.InitServer();
	server.RunServer();

	return 0;
}


void ErrorHandling(const char* msg)
{
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}