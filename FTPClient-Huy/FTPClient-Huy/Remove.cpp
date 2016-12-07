#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include "Remove.h"
#include "Display.h"

void Remove(SOCKET* controlConnectSocket)
{
	char deleteCommand[20];
	char filename[30];
	int isFolder = true;

	char recvBuffer[512];
	int recvBytes;
	
	Display(controlConnectSocket);

	printf("Type the filename you want to delete: \n");
	scanf("%s", &filename);

	// Kiểm tra xem tên vừa nhập là thư mục hay tệp
	for (int i = 0; i < strlen(filename); i++)
	{
		if (filename[i] == '.')
		{
			isFolder = false;
			break;
		}
	}
	
	if (isFolder == false)
	{ 
		//DELE <filename>
		sprintf(deleteCommand, "DELE %s\n", filename);

		send(*controlConnectSocket, deleteCommand, strlen(deleteCommand), 0);
		recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
		recvBuffer[recvBytes] = '\0';

		//nhan ket qua tu server 
		if (strncmp(recvBuffer, "250", 3) == 0)
		{
			printf("File is deleted successfully\n");
			return;
		}
		else if (strncmp(recvBuffer, "550", 3) == 0)
		{
			printf("File not founnd!\n");
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		//RMD <filename>
		sprintf(deleteCommand, "RMD %s\n", filename);

		send(*controlConnectSocket, deleteCommand, strlen(deleteCommand), 0);
		recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
		recvBuffer[recvBytes] = '\0';

		//nhan ket qua tu server 
		if (strncmp(recvBuffer, "250", 3) == 0)
		{
			printf("Folder is deleted successfully\n");
			return;
		}
		else if (strncmp(recvBuffer, "550", 3) == 0)
		{
			printf("Folder not founnd!\n");
			return;
		}
		else
		{
			return;
		}
	}

}