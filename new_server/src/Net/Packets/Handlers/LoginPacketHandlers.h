#pragma once

#include <string>

#include "./PacketHandler.h"
#include "../InPacket.h"

class LoginRequestHandler : public PacketHandler
{
public:
	void handle(InPacket pPacket, SockData* pSockData);
private:
};
