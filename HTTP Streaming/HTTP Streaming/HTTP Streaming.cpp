// HTTP Streaming.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <stdio.h>
#include <Windows.h>

DWORD WINAPI ClientThread(LPVOID lpParameter);
WIN32_FIND_DATAA Data;

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

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	SOCKET clientSocket = *(SOCKET *)lpParameter;
	char recvBuf[512], sendBuf[4086];
	int recvBufLen, sendBufLen;

	// Server receive request from browser
	recvBufLen = recv(clientSocket, recvBuf, strlen(recvBuf), 0);
	recvBuf[recvBufLen] = '\0';
	printf("%s\n\n", recvBuf[recvBufLen]);

	/* ====================================== 
		Get the path from browser's request
	    Ex: Get /ngoxuanhuy HTTP/1.1
	    => token = /ngoxuanhuy
	   ===================================== */
	if (strncmp(recvBuf, "GET", 3) == 0)
	{
		char *token = strtok(recvBuf, " ");
		token = strtok(NULL, " ");
		printf("%s\n", token);

		
		strcpy(sendBuf, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n");
		strcat(sendBuf, "<html><body>");

		HANDLE h = FindFirstFileA("D:\\ngoxuanhuy\\*.*", &Data);
		do {
			if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sendBuf, "<a href=\"#\" style=\"text-decoration: none; color: #00695C\"><b>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</b></a><br/>");
			}
			else
			{
				printf("Dont");
				strcat(sendBuf, "<a href=\"#\" style=\"text-decoration: none; color: #F50057\"><i>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</i></a><br/>");
			}
		} while (FindNextFileA(h, &Data));

		strcat(sendBuf, "</body></html>");
		send(clientSocket, sendBuf, strlen(sendBuf), 0);
	}
	closesocket(clientSocket);
}
