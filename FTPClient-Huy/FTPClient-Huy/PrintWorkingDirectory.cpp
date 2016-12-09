#pragma once
#include "stdafx.h"
#include "PrintWorkingDirectory.h"

void PrintWorkingDirectory(SOCKET *controlConnectionSocket)
{
	char recvCommand[100];
	char resutlStatement[100];

	int recvCommandLength;

	// Gui lenh toi server de xem thu muc hien tai
	send(*controlConnectionSocket, "pwd\n", 4, 0);

	// phan hoi cua server
	recvCommandLength = recv(*controlConnectionSocket, recvCommand, strlen(recvCommand), 0);
	recvCommand[recvCommandLength] = '\0';

	if (strncmp(recvCommand, "257", 0) == 0)
	{
		for (int i = 4; i <= strlen(recvCommand); i++)
		{
			resutlStatement[i - 4] = recvCommand[i];
		}
	}
	printf("%s\n", resutlStatement);
	return;
}