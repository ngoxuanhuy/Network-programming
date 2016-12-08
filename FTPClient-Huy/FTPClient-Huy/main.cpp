#include "stdafx.h"
#include <WinSock2.h>

#include "InitializeConnection.h"
#include "Login.h"
#include "Rename.h"
#include "Display.h"
#include "Download.h"
#include "Upload.h"
#include "Remove.h"
#include "CreateDirectory.h"
#include "ParallelDownload.h"
#include "CloseConnection.h"

#define SOCKET_DEPRECATED_NO_WARNING
#define WINSOCK_DEPRECATED_NO_WARNINGS

SOCKET controlConnectSocket;
char username[100];
char password[100];

int main(int argc, char* argv[])
{
	if (InitializeConnection(&controlConnectSocket) == false)
	{
		return 1;
	}
	bool checkLogin = FALSE;
	while (!checkLogin)
	{
		checkLogin = Login(&controlConnectSocket, username, password);
	}
	
	int option;
	do
	{
		printf("\t\t======================================= =======================================\n");
		printf("\t\t||       Select from 1 to 8            |                 GROUP 7             ||\n");
		printf("\t\t======================================= =======================================\n");
		printf("\t\t||  1.Display files in home directory  |                                     ||\n"
			   "\t\t||  2.Download                         |                                     ||\n"
			   "\t\t||  3.Multiple Download                |        1. Hoang Hiep                ||\n"
			   "\t\t||  4.Upload                           |        2. Ngo Xuan Huy              ||\n"
			   "\t\t||  5.Rename                           |        3. Tong Huu Dang Khoa        ||\n"
			   "\t\t||  6.Remove                           |                                     ||\n" 
			   "\t\t||  7.Make directory                   |                                     ||\n"
			   "\t\t||  8.Quit                             |                                     ||\n");
		printf("\t\t======================================= =======================================\n");

		fflush(stdin);
		scanf("%d", &option);

		switch (option)
		{
		case 1:
			Display(&controlConnectSocket);
			break;
		case 2:
			Download(&controlConnectSocket);
			break;
		case 3:
			ParallelDownload(&controlConnectSocket, username, password);
			break;
		case 4:
			Upload(&controlConnectSocket);
			break;
		case 5:
			Rename(&controlConnectSocket);
			break;
		case 6:
			Remove(&controlConnectSocket);
			break;
		case 7:
			CreateDirectory(&controlConnectSocket);
			break;
		default:
			break;
		}
	} while (option != 8);

	CloseConnection(controlConnectSocket);
	return 0;
}