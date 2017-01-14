#include "stdafx.h"
#include "Login.h"

bool Login(SOCKET* controlConnectSocket, char *_username, char *_password)
{
	char password[30];
	char userCommand[35];
	char passCommand[35];
	char recvBuffer[512];
	int recvBytes;

	while (true)
	{
		fflush(stdin);
		char username[30];
		printf("Username: ");
		scanf("%s", &username);

		//USER username
		sprintf(userCommand, "USER %s\n", username);

		send(*controlConnectSocket, userCommand, strlen(userCommand), 0);

		recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
		if (recvBytes > 0)
		{
			recvBuffer[recvBytes] = '\0';
			printf("%s", recvBuffer);
			recvBuffer[recvBytes] = '\0';
			// hoan thanh lenh USER <username>
			if (strncmp(recvBuffer, "331", 3) == 0)
			{
				printf("Password: ");
				scanf("%s", &password);

				sprintf(passCommand, "PASS %s\n", password);

				send(*controlConnectSocket, passCommand, strlen(passCommand), 0);

				recvBytes = recv(*controlConnectSocket, recvBuffer, 512, 0);
				if (recvBytes > 0)
				{
					recvBuffer[recvBytes] = '\0';
					if (strncmp(recvBuffer, "230", 3) == 0)
					{
						//--> dang nhap thanh cong
						printf("Logged On!\n");
						strcpy(_username, username);
						strcpy(_password, password);
						return true;
						
					}
					else
					{
						printf("Log in failed, check your username and password\n");
						return false;
					}
				}
			}
		}
	}
}