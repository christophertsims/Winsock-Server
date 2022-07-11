#include "PacketSwitch.h"

PacketSwitch::PacketSwitch()
{

}

void PacketSwitch::forward(const int8_t* pBytes, size_t length, SockData* pSockData)
{
	InPacket packet = { pBytes , length };

	int16_t op_code = packet.read_short();

	switch (op_code)
	{
	case 1:
		LoginRequestHandler packet_handler;
		packet_handler.handle(packet, pSockData);
		break;
	}
}