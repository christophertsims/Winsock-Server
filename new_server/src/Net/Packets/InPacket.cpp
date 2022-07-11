#include "InPacket.h"

InPacket::InPacket(const int8_t* recv, size_t length)
{
	bytes = recv;
	top = length;
	pos = 0;
}

size_t InPacket::length() const
{
	return top - pos;
}


void InPacket::skip(size_t count)
{
	if (count > length())
		throw "Stack overflow";

	pos += count;
}

int8_t InPacket::read_byte()
{
	return read<int8_t>();
}

int16_t InPacket::read_short()
{
	return read<int16_t>();
}

int32_t InPacket::read_int()
{
	return read<int32_t>();
}

int64_t InPacket::read_long()
{
	return read<int64_t>();
}

std::string InPacket::read_string()
{
	uint16_t str_len = read<uint16_t>();
	return read_padded_string(str_len);
}

// PRIVATE

std::string InPacket::read_padded_string(uint16_t count)
{
	std::string ret;

	for (int16_t i = 0; i < count; i++)
	{
		char letter = read_byte();

		if (letter != '\0')
			ret.push_back(letter);
	}

	return ret;
}