#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <process.h>

#pragma warning(disable:4996)

const static int BUF_SIZE = 100;
const static int  NAME_SIZE = 20;

unsigned WINAPI SendMsg(void* arg);
unsigned WINAPI RecvMsg(void* arg);
void ErrorHandleing(const char* msg);

std::string ip;
char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main()
{
	std::cout << "Input IP : ";
	std::cin >> ip;

	int portNum = 0;
	std::cout << "Input portNum : ";
	std::cin >> portNum;

	std::cout << "Input Name : ";
	char tempName[NAME_SIZE];
	std::cin >> tempName;
	sprintf(name, "[%s]", tempName);

	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandleing("WSAStartUp() error");

	hSock = socket(PF_INET, SOCK_STREAM, 0);

	ZeroMemory(&servAdr, 0);
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(ip.c_str());
	servAdr.sin_port = htons(portNum);

	if (connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandleing("connet() error");

	hSndThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);
	hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);

	WaitForSingleObject(hSndThread, INFINITE);
	WaitForSingleObject(hRcvThread, INFINITE);

	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void* arg)
{
	SOCKET hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	while (1)
	{
		fgets(msg, BUF_SIZE, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg, "%s %s", name, msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg)
{
	int hSock = *((SOCKET*)arg);
	char nameMsg[NAME_SIZE + BUF_SIZE];
	int strLen = 0;

	while (1)
	{
		strLen = recv(hSock, nameMsg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (strLen == -1)
		{
			return -1;
		}
		nameMsg[strLen] = 0;
		fputs(nameMsg, stdout);
	}

	return 0;
}

void ErrorHandleing(const char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}