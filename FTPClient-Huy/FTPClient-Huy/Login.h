#pragma once
#include <WinSock2.h>
#include <stdio.h>

// Dang nhap
bool Login(SOCKET* controlConnectSocket, char *username, char *password);
