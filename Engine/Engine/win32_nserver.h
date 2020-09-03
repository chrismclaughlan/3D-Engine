#pragma once
#include "win32_network.h"

class NServer : public Network
{
public:
	NServer(uint16 port);
	~NServer();
	void listen();
	bool isOnline();
};