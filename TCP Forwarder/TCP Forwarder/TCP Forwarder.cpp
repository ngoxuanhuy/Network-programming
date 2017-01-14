#include "stdafx.h"
#include <WinSock2.h>

int cIndex1 = 0;
SOCKET connectedSocket1[25];

DWORD WINAPI ClientThread0(LPVOID);
DWORD WINAPI ClientThread1(LPVOID);

int main()
{
	// Create window socket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create two server's addresses waiting at port 8000 and 8001
	// ... server address waiting at port 8000
	SOCKADDR_IN serverAddr0;
	serverAddr0.sin_family = AF_INET;
	serverAddr0.sin_addr.s_addr = INADDR_ANY;
	serverAddr0.sin_port = htons(8000);

	// ... server address waiting at port 8001
	SOCKADDR_IN serverAddr1;
	serverAddr1.sin_family = AF_INET;
	serverAddr1.sin_addr.s_addr = INADDR_ANY;
	serverAddr1.sin_port = htons(8001);

	// Create 2 sockets connect to 2 servers
	SOCKET listeningSocket0 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listeningSocket0, (SOCKADDR *)&serverAddr0, sizeof(serverAddr0));
	listen(listeningSocket0, 1);

	SOCKET listeningSocket1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listeningSocket1, (SOCKADDR *)&serverAddr1, sizeof(serverAddr1));
	listen(listeningSocket1, 25);

	// Create 1 thread to handle only one connection to port 8000
	SOCKADDR_IN clientAddr0;
	int clientAddrLen0 = sizeof(clientAddr0);
	SOCKET clientSocket0 = accept(listeningSocket0, (SOCKADDR *)&clientAddr0, &clientAddrLen0);
	CreateThread(0, 0, ClientThread0, (LPVOID)&clientSocket0, 0, 0);

	// Create a bunch of threads for handling connections to port 8001
	while (true)
	{
		SOCKADDR_IN clientAddr1;
		int clientAddrLen1 = sizeof(clientAddr1);
		SOCKET clientSocket1 = accept(listeningSocket1, (SOCKADDR *)&clientAddr1, &clientAddrLen1);
		CreateThread(0, 0, ClientThread1, (LPVOID)&clientSocket1, 0, 0);
	}
	return 0;
}

// If one connection closes, remove its socket
void removeSocket(SOCKET disconnectedSocket)
{
	int i = 0;
	for (; i < cIndex1; i++)
		if (connectedSocket1[i] == disconnectedSocket)
			break;
	if (i < cIndex1)
	{
		for (int j = i; j < cIndex1 - 1; j++)
		{
			connectedSocket1[j] = connectedSocket1[j + 1];
			cIndex1--;
		}
	}
}

// Thread for port 8001
DWORD WINAPI ClientThread1(LPVOID lpParameter)
{
	SOCKET connectedSocket = *(SOCKET *)lpParameter;
	printf("Socket 1.%d: connection\n", cIndex1);

	// Add this socket to an array of already connected socket on port 80001
	connectedSocket1[cIndex1] = connectedSocket;
	cIndex1++;

	int recvBufLen = 0;
	char recvBuf[512];
	while (true)
	{
		// if socket is disconnected => recvBufLen = 0
		// then close that socket and remove from array
		recvBufLen = recv(connectedSocket, recvBuf, strlen(recvBuf), 0);
		if (recvBufLen == 0)
		{
			closesocket(connectedSocket);
			removeSocket(connectedSocket);
			break;
		}
	}
	return 0;
}

// Thread for port 8000
DWORD WINAPI ClientThread0(LPVOID lpParameter)
{
	SOCKET connectedSocket = *(SOCKET *)lpParameter;
	printf("SOCKET AT port 8000 is connected\n");
	char recvBuf[512];
	int recvBufLen = 0;
	while (true)
	{
		recvBufLen = recv(connectedSocket, recvBuf, strlen(recvBuf), 0);
		if (recvBufLen > 0)
		{
			recvBuf[recvBufLen] = '\0';
			// Send message to all other clients
			for (int i = 0; i < cIndex1; i++)
			{
				send(connectedSocket1[i], recvBuf, strlen(recvBuf), 0);
			}
		}
		// Disconnect on port 8000
		else
		{
			closesocket(connectedSocket);
			break;
		}
	}
}