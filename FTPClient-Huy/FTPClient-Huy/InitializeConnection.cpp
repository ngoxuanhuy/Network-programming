#include "stdafx.h"
#include <WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string.h>
#include "InitializeConnection.h"


bool InitializeConnection(SOCKET* controlConnectSocket)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("WSAStartup error %d\n", GetLastError());
		return false;
	}
	//socket dieu khien ket noi
	*controlConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(21);//control-connection port
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (!connect(*controlConnectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		int recvLength;
		char buffer[512];
		recvLength = recv(*controlConnectSocket, buffer, strlen(buffer), 0);
		buffer[recvLength] = '\0';
		printf("%s",buffer);
		
		/*for (int i = 0; i < 4; i++)
		{
			recvLength = recv(*controlConnectSocket, buffer, strlen(buffer), 0);
			buffer[recvLength] = '\0';
			printf("%s", buffer);
		}*/
	}
	else
	{
		printf("Cannot connect to server!\n");
		return false;
	}
	return true;
}