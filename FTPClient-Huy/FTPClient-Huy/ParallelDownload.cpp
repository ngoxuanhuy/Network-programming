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
int numberOfSegments, segmentLength, finalSegmentLength;
int contentLength;
int newPosition[10];
char *position;
int recvThread[10];

SOCKET dataSocket[10];
SOCKET controlSocket[10];
int length[10];
SOCKET *currentControlSocket;

FILE* segmentFiles[10];
FILE* rootStage;

HANDLE H[11];

bool LoginAndDownloadAutomatic(int);

void ParallelDownload(SOCKET *controlConnectionSocket, char *_username, char *_password)
{
	fflush(stdin);
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
	numberOfSegments = 5;

	if ((fileSize % numberOfSegments) == 0)
	{
		segmentLength = fileSize / numberOfSegments;
		finalSegmentLength = segmentLength;
	}
	else
	{
		segmentLength = fileSize / numberOfSegments + 1;
		finalSegmentLength = fileSize - segmentLength * (numberOfSegments - 1);
	}

	// Tiến hành tải
	printf("Downloading....\n");

	for (int i = 1; i <= numberOfSegments; i++)
	{
		if (i != numberOfSegments)
		{
			length[i] = segmentLength;
		}
		else
		{
			length[i] = finalSegmentLength;
		}
		H[i] = CreateThread(0, 0, SegmentThread, (char *)i, 0, 0);

	}

	for (int i = 1; i <= numberOfSegments; i++)
	{
		WaitForSingleObject(H[i], INFINITE);
	}
	printf("Download segments successful...\n");

	// Tiến hành ghép nối các tệp tin
	Merging();
	fclose(rootStage);

	printf("Download totally success\n");

	return;
}

DWORD WINAPI SegmentThread(LPVOID lpParameter)
{
	int recvBytes = 0;
	char recvBuf[1024];
	char *i = (char *)lpParameter;
	int index = (int)i;

	bool check = false;

	//Neu dang nhap chua thanh cong => tiep tuc dang nhap lai
	while (check == false)
	{
		check = LoginAndDownloadAutomatic(index);
	}
	return 0;
}

bool LoginAndDownloadAutomatic(int index)
{
	InitializeConnection(controlSocket + index);

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
	if (recvThread[index] > 0)
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
		newPosition[index] = (index - 1) * segmentLength + 0;
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
				// Tạo một tệp tin tạm để lưu: filename[1] = "1"
				strcpy(fileNames[index], strIndex);

				// Mở tệp này để ghi dữ liệu
				segmentFiles[index] = fopen(fileNames[index], "wb");

				recvThread[index] = 0;
				/* Tiến hành tải dữ liệu của một phân mảnh,
				Ghi vào tệp vừa tạo */

				//Nếu độ dài của một phân mảnh < 1024 bytes...
				if (length[index] < 1024)
				{
					// ... thì tải một lần là xong
					recv(dataSocket[index], recvBuf[index], length[index], 0);
					recvBuf[index][length[index]] = '\0';
					fwrite(recvBuf[index], 1, length[index], segmentFiles[index]);
					fclose(segmentFiles[index]);
					break;
				}
				// ... hoặc độ dài của một phân mảnh = 1024 bytes...
				else if (length[index] == 1024)
				{
					// ... thì cũng tải một lần là xong
					recv(dataSocket[index], recvBuf[index], 1024, 0);
					fwrite(recvBuf[index], 1, sizeof(recvBuf[index]), segmentFiles[index]);
					fclose(segmentFiles[index]);
				}
				// ...nếu độ dài của một phân mảnh > 1024 bytes...
				else
				{
					// .. thì tải làm nhiều lần
					int reminder = length[index] % 1024;
					int div = length[index] / 1024;

					for (int i = 1; i <= div; i++)
					{
						recv(dataSocket[index], recvBuf[index], 1024, 0);
						fwrite(recvBuf[index], 1, sizeof(recvBuf[index]), segmentFiles[index]);
					}

					recv(dataSocket[index], recvBuf[index], reminder, 0);
					recvBuf[index][reminder] = '\0';
					fwrite(recvBuf[index], 1, reminder, segmentFiles[index]);
					fclose(segmentFiles[index]);
				}
				break;
			}
		}
		return true;
	}
	return false;
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



void Merging()
{
	// Ghép dữ liệu vào tệp tin gốc
	fflush(stdin);
	for (int i = 1; i <= numberOfSegments; i++)
	{
		FILE *fread = fopen(fileNames[i], "rb");
		while (fgets(line, sizeof(line), fread))
		{
			fputs(line, rootStage);
		}
		fclose(fread);
		remove(fileNames[i]);
		printf("Finish stage: %d\n", i);
		closesocket(dataSocket[i]);
		closesocket(controlSocket[i]);
	}

}