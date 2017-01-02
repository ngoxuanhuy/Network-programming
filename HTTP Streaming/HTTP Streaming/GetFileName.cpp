#include "stdafx.h"
#include "GetFileName.h"

char *GetFileName(char * pathName)
{
	char *token = strtok(pathName, "\\");
	char fileName[60];

	while (token != NULL)
	{
		strcpy(fileName, token);
		token = strtok(NULL, "\\");
	}
	return fileName;
}