#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "../SockData.h"
#include "./InPacket.h"

class OutPacket
{
public:
	OutPacket(int16_t opcode);

	void dispatch(SockData* pSockData);

	enum Opcode : uint16_t
	{
		// Good Packets

		/// Successfull login
		SUCCESSFULL_LOGIN = 1,


		// Bad Packets
		BAD_LOGIN = 2,
	};

	void skip(size_t count);
	void write_byte(int8_t ch);
	void write_short(int16_t sh);
	void write_int(int32_t in);
	void write_string(const std::string& pStr);

	std::vector<int8_t> get_header();

private:
	std::vector<int8_t> bytes;
	int16_t opcode;
};

