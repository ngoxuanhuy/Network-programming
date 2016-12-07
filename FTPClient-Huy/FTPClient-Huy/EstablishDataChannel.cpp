#include "stdafx.h"
#include "EstablishDataChannel.h"
#include "GetPortNumber.h"

bool EstablishDataChannel(SOCKET* controlConnectSocket, SOCKET dataSocket)
{
	char pasvCommand[5];
	char recvBuffer[200];
	int recvBytes;

	strcpy(pasvCommand, "pasv");
	send(*controlConnectSocket, pasvCommand, 5, 0);

	recvBytes = recv(*controlConnectSocket, recvBuffer, 200, 0);
	recvBuffer[recvBytes] = '\0';

	if (strncmp(recvBuffer, "421", 3) == 0)
	{
		printf("server cannot create socket error 421\n");
		return false;
	}

	int dataPort = GetPortNumber(recvBuffer);

	SOCKADDR_IN serverAddress;//dia chi server voi dataport
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(dataPort);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (connect(dataSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		printf("cannot establish data channel, error : %d\n", GetLastError());
		return false;
	}
	return true;
}