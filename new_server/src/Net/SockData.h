#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdint>
#include <stdio.h>
#include <iostream>

#include "./ServerObjects/Player.h"
#include "./Packets/InPacket.h"

class SockData
{
public:
	SockData(pollfd* pSocket);

	size_t recieve();
	bool dispatch(int8_t* bytes, size_t length);

	Player* get_player();
	const int8_t* get_buffer();

private:
	pollfd* socket;
	int8_t _buffer[512 * 2];

	Player player;
};

