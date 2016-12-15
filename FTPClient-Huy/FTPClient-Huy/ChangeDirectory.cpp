#include "stdafx.h"
#include "ChangeDirectory.h"
#include "PrintWorkingDirectory.h"

void ChangeDirectory(SOCKET *controlConnectionSocket)
{
	char pathName[100];
	char cwdCommand[100];
	char recvCommand[100];
	int recvCommandLength;

	// Nhap duong dan toi thu muc can thay doi
	printf("Type path to the folder:");
	scanf("%s", &pathName);

	// Gui lenh toi server
	sprintf(cwdCommand, "cwd %s\n", pathName);
	send(*controlConnectionSocket, cwdCommand, strlen(cwdCommand), 0);

	// Nhan phan hoi tu phia server
	recvCommandLength = recv(*controlConnectionSocket, recvCommand, strlen(recvCommand), 0);
	recvCommand[recvCommandLength] = '\0';
	if (strncmp(recvCommand, "550", 3) == 0)
	{
		printf("/%s: directory not found\n", pathName);
	}
	else
	{
		//printf("/%s: is current directory\n", pathName);
		PrintWorkingDirectory(controlConnectionSocket, 0);
	}
	return;
}