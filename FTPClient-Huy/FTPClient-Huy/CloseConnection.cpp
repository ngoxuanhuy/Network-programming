#pragma once
#include "stdafx.h"
#include "CloseConnection.h"

void CloseConnection(SOCKET controlConnectSocket)
{
	closesocket(controlConnectSocket);
	WSACleanup();
}

