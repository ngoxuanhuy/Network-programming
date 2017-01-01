#include "stdafx.h"
#include <WinSock2.h>
#include "CLientThread.h"
#include "GetPathName.h"

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	WIN32_FIND_DATAA Data;

	SOCKET clientSocket = *(SOCKET *)lpParameter;
	char recvBuf[512], sendBuf[4086];
	int recvBufLen, sendBufLen;

	// Server receive request from browser
	recvBufLen = recv(clientSocket, recvBuf, strlen(recvBuf), 0);
	recvBuf[recvBufLen] = '\0';

	/* ======================================
	Get the path name from browser's request
	Ex: Get /ngoxuanhuy/folder1 HTTP/1.1
	=>  /ngoxuanhuy/folder1
	===================================== */

	if (strncmp(recvBuf, "GET", 3) == 0)
	{
		char *token = strtok(recvBuf, " ");
		token = strtok(NULL, " ");

		char *path = GetPathName(token);
		char *pathName = path;
		strcat(pathName, "\\*.*");

		strcpy(sendBuf, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n");
		strcat(sendBuf, "<html><body style=\" color: #FF5722\">");
		strcat(sendBuf, "<h2><strong>LIST OF FILES</strong></h2>");
		strcat(sendBuf, "<h3><i>File name in italic and red, Directory name in bold and green</i></h3><hr/>");

		HANDLE h = FindFirstFileA(pathName, &Data);
		do {
			if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char innerPathName[512];
				strcpy(innerPathName, token);
				strcat(innerPathName, Data.cFileName);
				strcat(sendBuf, "<a href=\"");
				strcat(sendBuf, innerPathName);
				strcat(sendBuf,"\" style=\"text-decoration: none; color: #00695C\"><b>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</b></a><br/>");
			}
			else
			{
				strcat(sendBuf, "<a href=\"#\" style=\"text-decoration: none; color: #F50057\"><i>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</i></a><br/>");
			}
		} while (FindNextFileA(h, &Data));

		strcat(sendBuf, "</body></html>");
		send(clientSocket, sendBuf, strlen(sendBuf), 0);
	}
	closesocket(clientSocket);
}