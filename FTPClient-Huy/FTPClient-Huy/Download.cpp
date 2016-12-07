#include "stdafx.h"
#include "Download.h"
#include "Display.h"
#include "EstablishDataChannel.h"

void Download(SOCKET *controlConnectionSocket)
{
	char filename[100];
	char retrCommand[100];
	char sizeCommand[100];
	char recvBuffer[1024];
	int recvBytes;
	int ret;
	int filesize;
	FILE* file;

	Display(controlConnectionSocket);

	printf("Type the file's name you want to download: ");
	scanf("%s", filename);

	// Send command: SIZE <file_name>
	sprintf(sizeCommand, "SIZE %s\n", filename);
	send(*controlConnectionSocket, sizeCommand, strlen(sizeCommand), 0);

	// Receive response: 213 <file_size>
	recvBytes = recv(*controlConnectionSocket, recvBuffer, 1024, 0);
	recvBuffer[recvBytes] = '\0';

	sscanf(recvBuffer, "%d %d", &ret, &filesize);

	SOCKET dataSocket;
	dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!EstablishDataChannel(controlConnectionSocket, dataSocket))
	{
		return;
	}

	//RETR <filename>
	sprintf(retrCommand, "RETR %s\n", filename);
	send(*controlConnectionSocket, retrCommand, strlen(retrCommand), 0);

	recvBytes = 0;
	FILE* downloadFile;
	downloadFile = fopen(filename, "wb");
	
	printf("Downloading...\n");
	fflush(stdin);

	// Bat dau tai tep tin o day
	while (recvBytes < filesize)
	{
		recvBytes += recv(dataSocket, recvBuffer, 1024, 0);
		fwrite(recvBuffer, 1, sizeof(recvBuffer), downloadFile);
	}
	fclose(downloadFile);
	closesocket(dataSocket);

	// Sau khi truyền kênh dữ liệu xong. Server lại gửi thông điệp xác nhân về kênh điều khiển
	char recvAccept[1024];
	//Receive response: Accep connection
	recvBytes = recv(*controlConnectionSocket, recvAccept, strlen(recvAccept), 0);
	recvAccept[recvBytes] = '\0';
	printf("%s",recvAccept);
	
}