#include "stdafx.h"
#include "GetPortNumber.h"

int GetPortNumber(char recvBuffer[1024])
{
	//227 entering passive mode (h1,h2,h3,h4,p1,p2)
	char* address;
	char* ptr;
	int result[6];
	int port;
	address = &recvBuffer[27];
	recvBuffer[strlen(recvBuffer)] = '\0';

	ptr = strtok(recvBuffer, ",)");

	for (int i = 0; i < 6; i++)
	{
		result[i] = atoi(ptr);
		ptr = strtok(NULL, ",)");
	}
	port = result[4] * 256 + result[5];
	
	return port;
}