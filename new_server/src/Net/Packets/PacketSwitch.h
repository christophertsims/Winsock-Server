#pragma once
#include <cstdint>

#include "./InPacket.h"
#include "./Handlers/PacketHandler.h"
#include "./Handlers/LoginPacketHandlers.h"
#include "../SockData.h"

class PacketSwitch
{
public:

	PacketSwitch();

	enum Opcode : uint16_t
	{
		LOGIN = 1,
	};

	void forward(const int8_t* pBytes, size_t length, SockData* pSockData);
private:
	std::unique_ptr<PacketHandler> handlers[500];
};

