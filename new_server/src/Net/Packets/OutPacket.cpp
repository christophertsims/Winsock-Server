#include "OutPacket.h"

#include "../Server.h"

OutPacket::OutPacket(int16_t opc) : opcode(opc)
{
	write_short(opcode);
}

void OutPacket::dispatch(SockData* pSockData)
{

	pSockData->dispatch(get_header().data(), 2);
	pSockData->dispatch(bytes.data(), bytes.size());
}

void OutPacket::skip(size_t count)
{
	for (size_t i = 0; i < count; i++)
		bytes.push_back(0);
}

void OutPacket::write_byte(int8_t pByte)
{
	bytes.push_back(pByte);
}

void OutPacket::write_short(int16_t pShort)
{
	for (size_t i = 0; i < sizeof(short); i++)
	{
		write_byte(static_cast<int8_t>(pShort));
		pShort >>= 8;
	}
}

void OutPacket::write_int(int32_t pInt)
{
	for (size_t i = 0; i < sizeof(int); i++)
	{
		write_byte(static_cast<int8_t>(pInt));
		pInt >>= 8;
	}
}

void OutPacket::write_string(const std::string& pStr)
{
	int16_t str_len = static_cast<int16_t>(pStr.length());
	write_short(str_len);

	for (int16_t i = 0; i < str_len; i++)
	{
		write_byte(pStr[i]);
	}
}

std::vector<int8_t> OutPacket::get_header()
{
	std::vector<int8_t> header;
	int size = bytes.size();

	for (size_t i = 0; i < sizeof(short); i++)
	{
		header.push_back(static_cast<int8_t>(size));
		size >>= 8;
	}

	return header;
}