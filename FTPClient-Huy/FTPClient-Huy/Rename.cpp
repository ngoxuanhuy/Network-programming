#include "stdafx.h"
#include "Rename.h"
#include "Display.h"

void Rename(SOCKET* controlConnectSocket)
{
	char rnfrCommand[30];
	char rntoCommand[30];
	char oldName[60];
	char newName[60];

	char recvBuffer[512];
	int recvBytes;
	system("cls");
	Display(controlConnectSocket);

	printf("Type a filename you want to rename \n");
	scanf("%s", &oldName);

	//RNFR <filename>
	sprintf(rnfrCommand, "RNFR %s\n", oldName);

	//recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);//---> doan nay la chua loi server gui 2 dong 226, 350

	//gui ten file muon doi ten len server
	send(*controlConnectSocket, rnfrCommand, strlen(rnfrCommand), 0);
	recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
	recvBuffer[recvBytes] = '\0';

	//nhan ket qua tra ve tu server
	if (strncmp(recvBuffer, "350", 3) == 0)//success
	{
		printf("File name exists. Ready to rename:\n");
		printf("Type a new filename: \n");
		scanf("%s", &newName);

		//RNTO <filename>
		sprintf(rntoCommand, "RNTO %s\n", newName);
		//gui ten moi 
		send(*controlConnectSocket, rntoCommand, strlen(rntoCommand), 0);
		recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);

		printf("Renaming file succeeded\n");
	}
	else if (strncmp(recvBuffer, "550", 3) == 0)
	{
		printf("File not found\n");
	}
}