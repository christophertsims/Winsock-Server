#pragma once

#include "Poco/Data/RecordSet.h"
#include "./Poco/Data/Session.h"
#include "../../ServerObjects/Player.h"
#include "../OutPacket.h"
#include "../InPacket.h"

class PacketHandler
{
public:
	virtual void handle(InPacket pPacket, SockData* pSockData) = 0;
private:
};

