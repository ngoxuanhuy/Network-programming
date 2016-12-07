#pragma once
#include <WinSock2.h>
#include <stdio.h>

void ParallelDownload(SOCKET *controlConnectSocket, char *, char*);
DWORD WINAPI StageThread(LPVOID lpParameter);
void DownloadFinalStage();
void Connecting();
void ChangeStartPosition(SOCKET *controlConnectionSocket, int startPosition);