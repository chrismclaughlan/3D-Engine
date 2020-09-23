#include "win32_nserver.h"
#include "exception.h"

NServer::NServer(uint16 port)
{
	this->localAddr.sin_family = AF_INET;
	this->localAddr.sin_addr.s_addr = INADDR_ANY;
	this->localAddr.sin_port = htons(port);

	incomingAddr = {};

	int res;
	WSADATA wsaData;
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: WSAStartup failed");
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == sock)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: socket() returned INVALID_SOCKET");
	}

	res = bind(sock, (struct sockaddr*) & localAddr, sizeof(localAddr));
	if (SOCKET_ERROR == res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: bind() returned SOCKET_ERROR");
	}

	uint64 Blocking = 0;
	uint64 nonBlocking = 1;
	res = ioctlsocket(sock, FIONBIO, &nonBlocking);
	if (SOCKET_ERROR == res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error initialising server: ioctlsocket() returned SOCKET_ERROR");
	}
}

NServer::~NServer()
{
	closesocket(sock);
	WSACleanup();
}

void NServer::listen()
{

}

bool NServer::isOnline()
{
	return true;  // todo ping?
}