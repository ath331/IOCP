#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <process.h>

const static int BUF_SIZE = 100;
const static int  MAX_CLNT = 256;

unsigned WINAPI HandleClnt(void* arg);
void SendMsg(const char* msg, int len);
void ErrorHandleing(const char* msg);

int clntCnt = 0;
SOCKET clientSockets[MAX_CLNT];
HANDLE hMutex;

int main(int argc, const char* argv[])
{
	WSADATA wsaData;
	SOCKET hServerSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz = 0;
	HANDLE hThread;
	
	if (argc != 2)
	{
		std::cout << "Usage : " << argv[0] << " <port> " << std::endl;
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandleing("WSAStartUp() error");

	hMutex = CreateMutex(NULL, FALSE, NULL);
	hServerSock = socket(PF_INET, SOCK_STREAM, 0);

	ZeroMemory(&hServerSock, 0);
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServerSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandleing("bind() error");
	if (listen(hServerSock, 5) == SOCKET_ERROR)
		ErrorHandleing("listen() error");

	while (1)
	{
		clntAdrSz = sizeof(clntAdr);
		hClntSock = accept(hServerSock, (SOCKADDR*)&clntAdr, &clntAdrSz);

		WaitForSingleObject(hMutex, INFINITE);
		clientSockets[clntCnt++] = hClntSock;
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);
		std::cout << "Connet client IP : " << inet_ntoa(clntAdr.sin_addr) << std::endl;
	}
	
	closesocket(hServerSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0;
	char msg[BUF_SIZE];

	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0)
	{
		SendMsg(msg, strLen);
	}

	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntCnt; i++)
	{
		if (hClntSock == clientSockets[i])
		{
			while (i++ < clntCnt - 1)
			{
				clientSockets[i] = clientSockets[i + 1];
			}
			break;
		}
	}
	clntCnt--;
	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;
}

void SendMsg(const char* msg, int len)
{
	int i = 0;
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
	{
		send(clientSockets[i],msg, len, 0);
	}
	ReleaseMutex(hMutex);
}

void ErrorHandleing(const char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}