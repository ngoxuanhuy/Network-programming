#include <WinSock2.h>
#include <stdio.h>

void ParallelDownload(SOCKET *controlConnectSocket, char *, char*);
DWORD WINAPI SegmentThread(LPVOID lpParameter);
void Merging();
void ChangeStartPosition(SOCKET controlConnectionSocket, int startPosition);
