#include "stdafx.h"
#include "GetPathName.h"
#include <string.h>

char *GetPathName(char * recvBuf)
{
	char pathName[512] = "C:\\";

	char *token = strtok(recvBuf, "/");

	while (token != NULL)
	{
		strcat(pathName, "\\");
		strcat(pathName, token);
		token = strtok(NULL, "/");
	}
	return pathName;
}