#pragma once
#include "types.h"
#include "network.h"
#include <winsock2.h>

#include <iostream>

// inet_pton
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define THROW_NETWORK_EXCEPTION(msg)\
THROW_EXCEPTION("Network Error", msg);
#define THROW_NETWORK_EXCEPTION_CODE(msg)\
THROW_EXCEPTION_CODE("Network Error", msg, WSAGetLastError());

class Network
{
protected:
	SOCKET sock;
	sockaddr_in localAddr;
	sockaddr_in incomingAddr;
public:
	virtual void deliver(NETWORK::Packet& packet);
	virtual bool receive(NETWORK::Packet& packet);
	virtual bool isOnline() = 0;

	void coutIncomingAddr() const noexcept
	{
		char buff[16];
		inet_ntop(incomingAddr.sin_family, &incomingAddr.sin_addr, buff, sizeof(buff));
		std::cout << "Received packet from " << buff << ":" << ntohs(incomingAddr.sin_port);
	}
};