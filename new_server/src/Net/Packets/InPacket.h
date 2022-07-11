#pragma once
#include <cstdint>
#include <string>
#include <iostream>

class InPacket
{
public:
	InPacket(const int8_t* recv, size_t length);
	size_t length() const;
	void skip(size_t count);
	int8_t read_byte();
	int16_t read_short();
	int32_t read_int();
	int64_t read_long();
	std::string read_string();
	std::string read_padded_string(uint16_t length);

private:
	template <typename T>
	// Read a number and advance the buffer position
	T read()
	{
		size_t count = sizeof(T) / sizeof(int8_t);
		T all = 0;

		for (size_t i = 0; i < count; i++)
		{
			T val = static_cast<uint8_t>(bytes[pos]);
			all += val << (8 * i);

			skip(1);
		}

		return static_cast<T>(all);
	}

	template <typename T>
	// Read without advancing the buffer position
	T inspect()
	{
		size_t before = pos;
		T value = read<T>();
		pos = before;

		return value;
	}

	const int8_t* bytes;
	size_t top;
	size_t pos;
};

