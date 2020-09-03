#include "win32_network.h"
#include "exception.h"
#include "types.h"
#include "defines.h"

#include <iostream>

// inet_pton
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

void Network::deliver(NETWORK::Packet& packet)
{
	if (packet.numBytes >= NETWORK::maxBufferSize)
	{
		packet.numBytes = NETWORK::maxBufferSize - 1;
	}

	int32 res, flags = 0;
	res = sendto(sock, packet.buffer, packet.numBytes, flags,
		(struct sockaddr*) & incomingAddr, (int32)sizeof(incomingAddr));

	if (SOCKET_ERROR == res)
	{
		THROW_NETWORK_EXCEPTION_CODE("Error delivering packet: sendto() returned SOCKET_ERROR");
	}
}

bool Network::receive(NETWORK::Packet& packet)
{
	int32 server_addr_size = sizeof(incomingAddr);
	packet.numBytes = recvfrom(sock, packet.buffer, sizeof(packet.buffer) - 1, 0,
		(struct sockaddr*) & incomingAddr, &server_addr_size);

	if (packet.numBytes == SOCKET_ERROR)
	{
		int32 WSALastError = WSAGetLastError();
		if (WSALastError == WSAEWOULDBLOCK)
		{
			return false;
		}
		else
		{
			THROW_NETWORK_EXCEPTION_CODE("Error receiving packet: recvfrom returned SOCKET_ERROR");
		}
	}

	//packet.buffer[NETWORK::maxBufferSize - 1] = 0;

//#if DISPLAY_DEBUG_CONSOLE
	//char buff[16];
	//inet_ntop(incomingAddr.sin_family, &incomingAddr.sin_addr, buff, sizeof(buff));
	//printf("Received packet from %s:%d\n", buff, ntohs(incomingAddr.sin_port));
	//printf("Data: %s\n", packet.buffer);
//#endif

	return true;
}