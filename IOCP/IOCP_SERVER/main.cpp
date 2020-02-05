#include <stdio.h>
#include <string>
#include <WinSock2.h>

const static int BUF_SIZE = 100;

void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);
void ErrorHandling(const char* msg);

int main(int argc, const char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;

	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT newEvent;
	WSANETWORKEVENTS netEvents;

	int numOfClntSock = 0;
	int strLen = 0, i = 0;
	int posInfo = 0, startIdx = 0;
	int clntAdrLen = 0;
	char msg[BUF_SIZE];

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error");
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		ErrorHandling("bind() error");
	}
	if (listen(hServSock, 5) == SOCKET_ERROR)
	{
		ErrorHandling("listen() error");
	}

	newEvent = WSACreateEvent();
	if (WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		ErrorHandling("WSAEventSelect() error");
	}

	hSockArr[numOfClntSock] = hServSock;
	hEventArr[numOfClntSock] = newEvent;
	numOfClntSock++;

	while (1)
	{
		posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);
		startIdx = posInfo - WSA_WAIT_EVENT_0;

		for (i = 0; i < numOfClntSock; i++)
		{
			int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);
			if ((sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT))
			{
				continue;
			}

			else
			{
				sigEventIdx = i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);

				if (netEvents.lNetworkEvents & FD_ACCEPT)
				{
					if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					{
						puts("ACCEPT error");
						break;
					}
					clntAdrLen = sizeof(clntAdr);
					hClntSock = accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent = WSACreateEvent();
					WSAEventSelect(hClntSock, newEvent, FD_READ | FD_CLOSE);

					hEventArr[numOfClntSock] = newEvent;
					hSockArr[numOfClntSock] = hClntSock;
					numOfClntSock++;
					puts("connect new client...");
				}

				else if (netEvents.lNetworkEvents & FD_READ)
				{
					if (netEvents.iErrorCode[FD_READ_BIT] != 0)
					{
						puts("READ error");
						break;
					}
					strLen = recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);
					send(hSockArr[sigEventIdx], msg, strLen, 0);
				}

				else if (netEvents.lNetworkEvents & FD_CLOSE)
				{
					if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
					{
						puts("CLOSE error");
						break;
					}
					WSACloseEvent(hEventArr[sigEventIdx]);
					closesocket(hSockArr[sigEventIdx]);

					numOfClntSock--;
					CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
					CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
				}
			}
		}
	}

	WSACleanup();
	return 0;
}

void CompressSockets(SOCKET hSockArr[], int idx, int total)
{
	int i = 0;
	for (i = idx; i < total; i++)
	{
		hSockArr[i] = hSockArr[i + 1];
	}
}
void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
	int i = 0;
	for (i = idx; i < total; i++)
	{
		hEventArr[i] = hEventArr[i + 1];
	}
}
void ErrorHandling(const char* msg)
{
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}