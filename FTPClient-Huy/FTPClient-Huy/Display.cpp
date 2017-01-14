#include "stdafx.h"
#include <WinSock2.h>
#include "Display.h"
#include "EstablishDataChannel.h"

void Display(SOCKET* controlConnectSocket)
{
	char pasvCommand[5];
	char listCommand[10];
	char recvBuffer[512];
	char dataBuffer[2048];
	int recvBytes;

	SOCKET dataSocket;
	dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Mo kenh du lieu
	EstablishDataChannel(controlConnectSocket, dataSocket);
	strcpy(listCommand, "LIST\n");
	send(*controlConnectSocket, listCommand, strlen(listCommand), 0);

	// Kênh điều khiển nhận thông báo kết nối tới kênh dữ liệu thành công hay thất bại
	recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
	recvBuffer[recvBytes] = '\0';
	//printf("%s", recvBuffer);

	char recvAccept[1024];
	if (strncmp(recvBuffer, "150", 3) == 0)
	{
		recvBytes = recv(dataSocket, dataBuffer, 2048, 0);
		dataBuffer[recvBytes] = '\0';

		printf("%s\n", dataBuffer);
		closesocket(dataSocket);
	}

	recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
	recvBuffer[recvBytes] = '\0';
	//printf("%s", recvBuffer);
	
}