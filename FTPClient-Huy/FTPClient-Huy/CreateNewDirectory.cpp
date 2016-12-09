#include "stdafx.h"
#include "CreateNewDirectory.h"
#include <stdlib.h>

void CreateNewDirectory(SOCKET *controlConnectionSocket)
{
	char CreateCommand[50];
	char folderName[30] = "huy huy";
	char recvBuffer[512];

	int recvBufLength;
	int folderNameLength = 30;

	// Nhap ten thu muc moi muon tao
	printf("Ten thu muc moi muon tao: ");
	scanf("%s", &folderName);
	
	printf("new folder you want to create: %s", folderName);
	
	// Cau lenh tao thu muc
	sprintf(CreateCommand,"MKD %s\n",folderName);
	
	// Gui lenh toi server
	send(*controlConnectionSocket, CreateCommand, strlen(CreateCommand), 0);

	// Nhan phan hoi tu server
	recvBufLength = recv(*controlConnectionSocket, recvBuffer, strlen(recvBuffer), 0);
	if (strncmp(recvBuffer, "257", 3) == 0)
	{
		printf("Created new folder successfully\n");
		return;
	}
	// Neu thu muc da ton tai
	if (strncmp(recvBuffer, "550", 3) == 0)
	{
		printf("Directory already exists\n");
		return;
	}
}