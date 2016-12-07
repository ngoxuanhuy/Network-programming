#include "stdafx.h"
#include "ParallelDownload.h"
#include "GetPortNumber.h"
#include "Display.h"
#include "EstablishDataChannel.h"
#include "InitializeConnection.h"

char fileName[100];
char retrCommand[100];
char sizeCommand[100];
char fileNames[10][100];
char recvBuf[5][1024];
char line[1024];
char recvBuffer[4096];
char localUsername[100];
char localPassword[100];

int recvBytes;
int ret;
int fileSize;
int numberOfStages, stageLength, finalStageLength;
int contentLength[10];
int newPosition[10];
char *position;
int recvThread[10];

SOCKET dataSocket[10];
SOCKET controlSocket[10];
SOCKET *currentControlSocket;

FILE* downloadStage[10];
FILE* rootStage;

HANDLE H[11];

bool AutoLoginAndDownload(int, int);

void ParallelDownload(SOCKET *controlConnectionSocket, char *_username, char *_password)
{
	
	// Yêu cầu xem các tệp tin đang có
	Display(controlConnectionSocket);

	// Nhập tên tệp tin muốn tải
	printf("Type the file's name you want to download: ");
	scanf("%s", fileName);

	// Mở tệp tin gốc tổng hợp các phân mảnh
	rootStage = fopen(fileName, "wb");

	// Gửi lệnh để xem dung lượng tệp tin
	sprintf(sizeCommand, "SIZE %s\n", fileName);
	send(*controlConnectionSocket, sizeCommand, strlen(sizeCommand), 0);

	recvBytes = recv(*controlConnectionSocket, recvBuffer, 1024, 0);
	recvBuffer[recvBytes] = '\0';

	// Gán kích thước tệp tin vào biến fileSize;
	sscanf(recvBuffer, "%d %d", &ret, &fileSize);

	// Gán các giá trị vào biến cục bộ
	currentControlSocket = controlConnectionSocket;
	strcpy(localUsername, _username);
	strcpy(localPassword, _password);

	// Số mảnh tệp tin bị chia
	//printf("Number of stages: ");
	//scanf("%d", &numberOfStages);
	numberOfStages = 3;

	// Dung lượng từng mảnh của tệp tin
	stageLength = fileSize / numberOfStages;

	// Dung lượng mảnh cuối cùng của tệp tin = Dung lượng còn lại chưa tải
	if (numberOfStages == 1)
		finalStageLength = stageLength;
	else
		finalStageLength = fileSize % numberOfStages;

	// Tiến hành tải
	printf("Downloading....\n");

	// Voi i = numberOfStages => Thread chi thuc hien tu 1 den numberOfStages - 1 ????
	for (int i = 1; i <= numberOfStages; i++)
	{
		H[i] = CreateThread(0, 0, StageThread, (char *)i, 0, 0);
		WaitForSingleObject(H[i], INFINITE);
	}

	//for (int i = 1; i <= numberOfStages; i++)
	//{
	//	printf("Finish stage: %d", i);
	//	// Ghép dữ liệu vào tệp tin gốc
	//	FILE *fread = fopen(fileNames[i], "rb");
	//	while (fgets(line, sizeof(line), fread))
	//	{
	//		fputs(line, rootStage);
	//	}
	//	fclose(fread);
	//	remove(fileNames[i]);
	//}
	fclose(rootStage);

	/*printf("Connecting....\n");
	for (int i = 1; i <= numberOfStages; i++)
	{
		FILE *fread = fopen(fileNames[i], "rb");
		while (fgets(line, sizeof(line), fread))
		{
			fputs(line, rootStage);
		}
		fclose(fread);
		remove(fileNames[i]);
	}*/
	printf("Download successfully...\n");
	return;
}

DWORD WINAPI StageThread(LPVOID lpParameter)
{
	int recvBytes = 0;
	char recvBuf[1024];
	char *i = (char *)lpParameter;
	int index = (int)i;
	
	if (index == numberOfStages)
		contentLength[index] = finalStageLength;
	else
		contentLength[index] = stageLength;

	bool check = false;

	//Neu dang nhap chua thanh cong => tiep tuc dang nhap lai
	while (check == false)
	{
		check = AutoLoginAndDownload(index, contentLength[index]);
	}
	//AutoLoginAndDownload(index, contentLength);
	return 1;
}

void ChangeStartPosition(SOCKET controlConnectionSocket, int startPosition)
{
	char restCommand[100];
	char recvResponse[100];

	// Cau lenh thay doi vi tri con tro trong tep tin
	sprintf(restCommand, "REST %d\n", startPosition);

	// Gui lenh toi server
	send(controlConnectionSocket, restCommand, strlen(restCommand), 0);

	// Nhan phan hoi tu server tra ve
	int recvBuf = recv(controlConnectionSocket, recvResponse, strlen(recvResponse), 0);
	recvResponse[recvBuf] = '\0';
	return;
}

bool AutoLoginAndDownload(int index, int length)
{
	InitializeConnection(controlSocket+index);

	char userCommand[35];
	char passCommand[35];
	char recvBuffer[1024];
	char strIndex[3];

	int nRet;
	unsigned long ul = 1;

	sprintf(strIndex, "%d", index);
	
	//USER username
	sprintf(userCommand, "USER %s\n", localUsername);

	send(controlSocket[index], userCommand, strlen(userCommand), 0);

	recvThread[index] = recv(controlSocket[index], recvBuffer, 512, 0);
	if (recvBytes > 0)
	{
		recvBuffer[recvThread[index]] = '\0';

		// hoan thanh lenh USER <username>
		if (strncmp(recvBuffer, "331", 3) == 0)
		{
			sprintf(passCommand, "PASS %s\n", localPassword);

			send(controlSocket[index], passCommand, strlen(passCommand), 0);

			recvThread[index] = recv(controlSocket[index], recvBuffer, 512, 0);
			if (recvThread[index] > 0)
			{
				recvBuffer[recvThread[index]] = '\0';
				if (strncmp(recvBuffer, "230", 3) == 0)
				{
					// dang nhap thanh cong
				}
				else
				{
					// dang nhap that bai
					closesocket(controlSocket[index]);
					return false;
				}
			}
		}

		// Gửi lệnh REST để thay đổi điểm bắt đầu của con trỏ trên tệp cần tải phía server
		newPosition[index] = (index - 1) * stageLength + 0;
		ChangeStartPosition(controlSocket[index], newPosition[index]);

		//* Tạo socket của đường truyền dữ liệu
		dataSocket[index] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (!EstablishDataChannel(controlSocket + index, dataSocket[index]))
		{
			printf("Cannot establish data channel");
			return false;
		}

		//Gửi lệnh tải tệp đển server: RETR <filename>
		sprintf(retrCommand, "RETR %s\n", fileName);
		send(controlSocket[index], retrCommand, strlen(retrCommand), 0);

		// Nhận kết quả phản hồi từ server: Accept connection
		while (true)
		{
			recvThread[index] = recv(controlSocket[index], recvBuffer, 512, 0);
			recvBuffer[recvThread[index]] = '\0';

			if (strncmp(recvBuffer, "150", 3) == 0)
			{
				// Tạo một tệp để lưu tạm dữ liệu của một phân mảnh: filename[1] = "1"
				strcpy(fileNames[index], strIndex);

				// Mở tệp này ra để ghi dữ liệu
				downloadStage[index] = fopen(fileNames[index], "wb");

				recvThread[index] = 0;
				// Tiến hành tải dữ liệu của 1 phân mảnh,
				// Ghi dữ liệu vào tệp vừa tạo
				while (recvThread[index] < length)
				{
					recvThread[index] += recv(dataSocket[index], recvBuf[index], 1024, 0);
					fwrite(recvBuf[index], 1, sizeof(recvBuf[index]), downloadStage[index]);
				}
				//fclose(downloadStage[index]);
				break;
			}
		}
		// Nhận kết quả phản hồi từ server: data transfer success
		/*recvBytes = recv(controlSocket[index], recvBuffer, 512, 0);
		closesocket(dataSocket[index]);
		closesocket(controlSocket[index]);*/
		return true;
	}
	return false;
}

