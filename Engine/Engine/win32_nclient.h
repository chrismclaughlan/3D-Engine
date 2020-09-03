#pragma once
#include "win32_network.h"

class NClient : public Network
{
public:
	NClient();
	~NClient();
	void connect(const char* sIP, const uint16 sPort);
	bool isOnline();
};