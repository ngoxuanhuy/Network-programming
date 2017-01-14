// HTTP Streaming.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include "ClientThread.h"

int main()
{
	// Create Window Socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Cannot create WinSock\n");
		return 1;
	}

	// Create server address
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(8000);

	// Create socket that connect to server
	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listeningSocket == INVALID_SOCKET)
	{
		printf("Cannot create socket\n");
		return 1;
	}

	// binding and listening
	bind(listeningSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
	listen(listeningSocket, 10);

	// Waiting for connection...
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(listeningSocket, (SOCKADDR *)&clientAddr, &clientAddrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("Cannot connect to client\n");
			return 1;
		}
		
		CreateThread(0, 0, ClientThread, (LPVOID)&clientSocket, 0, 0);	
		
	}
	closesocket(listeningSocket);
	WSACleanup();
    return 0;
}


