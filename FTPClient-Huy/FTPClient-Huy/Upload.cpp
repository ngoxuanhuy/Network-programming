#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include "Upload.h"
#include "EstablishDataChannel.h"
#include <iostream>
using namespace std;

void Upload(SOCKET *controlConnectionSocket)
{
	char filename[512];
	char pathName[1024];
	char userInput[100];
	char storCommand[100];
	char recvByte[100];
	char sendBuffer[1024];
	int recvBytes;
	int filesize;
	int sentBytes = 0;

	//liet ke danh sach file trong tep cua nguoi dung
	fflush(stdin);
	system("dir .\\File");

	// Nhập tên tệp muốn tải
	printf("Type the file's name you want to upload\n");
	scanf("%s", &filename);

	//sprintf(filename, "\data\%s", userInput);
	sprintf(pathName, "C:\\Users\\huynx\\Desktop\\FTP_v2\\source_from_git\\FTP\\FTPClient-Huy\\FTPClient-Huy\\File\\%s", filename);
	
	FILE* uploadFile;
	uploadFile = fopen(pathName, "rb");
	if (uploadFile == NULL)
	{
		printf("Cannot find the file path\n");
		fflush(stdin);
		return;
	}

	//tim filesize
	fseek(uploadFile, 0L, SEEK_END);
	filesize = ftell(uploadFile);
	rewind(uploadFile);

	//mo datasocket
	SOCKET dataSocket;
	dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EstablishDataChannel(controlConnectionSocket, dataSocket);

	//gui lenh
	//STOR <filename>
	sprintf(storCommand, "STOR %s\n", filename);
	send(*controlConnectionSocket, storCommand, strlen(storCommand), 0);

	// Sau khi truyền kênh dữ liệu xong. Server lại gửi thông điệp xác nhân về kênh điều khiển
	char recvAccept[1024];
	//Receive response: Accep connection
	recvBytes = recv(*controlConnectionSocket, recvAccept, strlen(recvAccept), 0);
	recvAccept[recvBytes] = '\0';

	if (strncmp(recvAccept, "150", 3) == 0)
	{
		printf("Uploading...\n");
		while (sentBytes < filesize)
		{
			int read = fread(sendBuffer, 1, 1024, uploadFile);
			if (read < 0)
			{
				break;
			}
			send(dataSocket, sendBuffer, read, 0);
			if (read < 1024)	// end of file
				break;
		}
		printf("Uploading finished\n");
		fclose(uploadFile);
		closesocket(dataSocket);

		char recvAccept1[1024];
		//Receive response: "226 Transfer OK"
		recvBytes = recv(*controlConnectionSocket, recvAccept1, strlen(recvAccept1), 0);
		recvAccept1[recvBytes] = '\0';
	}
}
