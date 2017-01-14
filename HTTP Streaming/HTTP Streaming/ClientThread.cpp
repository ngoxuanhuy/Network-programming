#include "stdafx.h"
#include <WinSock2.h>
#include "CLientThread.h"
#include "GetPathName.h"
#include "GetFileName.h"

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	WIN32_FIND_DATAA Data;

	SOCKET clientSocket = *(SOCKET *)lpParameter;
	char recvBuf[512], sendBuf[4096];
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
		char *tok = strtok(recvBuf, " ");
		tok = strtok(NULL, " ");
		char token[512];
		strcpy(token, tok);

		char *result = GetPathName(token, &clientSocket);
		char pathName[512], path[512];
		strcpy(pathName, result);
		strcpy(path, pathName);

		char *file = GetFileName(path);
		char fileName[512];
		strcpy(fileName, file);

		// Check if this file is a .txt file
		char *fileExtension = ".txt";
		char *firstOccurent = strstr(fileName, fileExtension);

		// No it is not
		if (firstOccurent != NULL)
		{
			strcpy(sendBuf, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n");
			strcat(sendBuf, "<html><body style=\" color: #5D4037\">");
			strcat(sendBuf, "<h2 style=\" color: #FF5722\"><strong>LIST OF FILES</strong></h2>");
			strcat(sendBuf, "<h3 style=\" color: #FF5722\"><i>File name in italic and red, Directory name in bold and green</i></h3><hr/>");
			strcat(sendBuf, "<h3>FileName: <i>");
			strcat(sendBuf, fileName);
			strcat(sendBuf, "</i></h3>");
			strcat(sendBuf, "<h3>Content</h3>");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);

			FILE *fout = fopen(pathName, "r");

			char line[1024];
			while (fgets(line, sizeof(line), fout))
			{
				strcpy(sendBuf, line);
				strcat(sendBuf, "<br/>");
				send(clientSocket, sendBuf, strlen(sendBuf), 0);
			}
			fclose(fout);

			strcpy(sendBuf, "</body></html>");
			send(clientSocket, sendBuf, strlen(sendBuf), 0);
			closesocket(clientSocket);
			return 0;
		}
		strcat(pathName, "\\*.*");
		strcpy(sendBuf, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n");
		strcat(sendBuf, "<html><body style=\" color: #FF5722\">");
		strcat(sendBuf, "<h2><strong>LIST OF FILES</strong></h2>");
		strcat(sendBuf, "<h3><i>File name in italic and red, Directory name in bold and green</i></h3><hr/>");

		HANDLE h = FindFirstFileA(pathName, &Data);
		do {
			char innerPathName[512];
			if (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy(innerPathName, token);
				if (strcmp(innerPathName, "/") != 0)
				{
					strcat(innerPathName, "/");
				}
				strcat(innerPathName, Data.cFileName);
				strcat(sendBuf, "<a href=\"");
				strcat(sendBuf, innerPathName);
				strcat(sendBuf,"\" style=\"text-decoration: none; color: #00695C\"><b>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</b></a><br/>");
			}
			else
			{
				strcpy(innerPathName, token);
				if (strcmp(innerPathName, "/") != 0)
				{
					strcat(innerPathName, "/");
				}
				strcat(innerPathName, Data.cFileName);
				strcat(sendBuf, "<a href=\"");
				strcat(sendBuf, innerPathName);
				strcat(sendBuf, "\" style=\"text-decoration: none; color: #F50057\"><i>");
				strcat(sendBuf, Data.cFileName);
				strcat(sendBuf, "</i></a><br/>");
			}
		} while (FindNextFileA(h, &Data));

		strcat(sendBuf, "</body></html>");
		send(clientSocket, sendBuf, strlen(sendBuf), 0);
	}
	closesocket(clientSocket);
}