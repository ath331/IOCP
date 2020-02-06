#include <stdio.h>
#include <string>
#include <WinSock2.h>

#pragma warning(disable:4996)

const static int BUF_SIZE = 100;
void ErrorHandling(const char* msg);

int main(int argc, const char* argv[])
{
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN sendAdr;

	WSABUF dataBuf;
	memset(&dataBuf, 0, sizeof(dataBuf));
	std::string msg = "Network is Computer!";
	int sendBytes = 0;

	WSAEVENT evObj;
	WSAOVERLAPPED overlapped;

	if (argc != 3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error");
	}

	hSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family = AF_INET;
	sendAdr.sin_addr.s_addr = inet_addr(argv[1]);
	sendAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSock, (SOCKADDR*)&sendAdr, sizeof(sendAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error");
	}

	evObj = WSACreateEvent();
	memset(&overlapped, 0, sizeof(overlapped));
	overlapped.hEvent = evObj;
	dataBuf.len = strlen(msg.c_str()) +1;
	dataBuf.buf = (char*)msg.c_str();

	if (WSASend(hSock, &dataBuf, 1, (LPDWORD)&sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSA_IO_PENDING)
		{
			puts("BackGround data send");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);
			WSAGetOverlappedResult(hSock, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);
		}
		else
		{
			ErrorHandling("WSASend() error");
		}
	}

	printf("Send data size : %d \n", sendBytes);
	WSACloseEvent(evObj);
	closesocket(hSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* msg)
{
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}