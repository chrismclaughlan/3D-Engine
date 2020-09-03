#include "win32_nclient.h"
#include "exception.h"

NClient::NClient()
{
	localAddr = {};
	incomingAddr = {};

	int32 res;
	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: WSAStartup failed");
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sock)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: socket() returned INVALID_SOCKET");
	}

	uint64 Blocking = 0;
	uint64 nonBlocking = 1;
	res = ioctlsocket(sock, FIONBIO, &nonBlocking);
	if (SOCKET_ERROR == res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: ioctlsocket() returned SOCKET_ERROR");
	}
}

NClient::~NClient()
{
	closesocket(sock);
	WSACleanup();
}

void NClient::connect(const char* sIP, const uint16 sPort)
{
	memset((char*)&this->incomingAddr, 0, sizeof(this->incomingAddr));
	incomingAddr.sin_family = AF_INET;
	incomingAddr.sin_port = htons(sPort);
	int32 result = inet_pton(AF_INET, sIP, &incomingAddr.sin_addr.s_addr);
	if (result != 1)
	{
		THROW_NETWORK_EXCEPTION("Error connecting to server: Not a valid IPv4 or IPv6 address");
	}
	else if (result != 1)
	{
		THROW_NETWORK_EXCEPTION("Error connecting to server");  // TODO implement WSAGetLastError()
	}
}

bool NClient::isOnline()
{
	return true;  // todo ping?
}