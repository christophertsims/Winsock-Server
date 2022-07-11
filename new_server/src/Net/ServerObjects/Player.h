#pragma once

#include <map>
#include <string>

#include <winsock2.h>

class Player
{
public:
	Player(SOCKET* pSocket);

	void set_username(std::string pUserName);
	void set_id(int pId);

private:
	//std::map<int, Character> characters;
	SOCKET* socket;
	std::string user_name;
	int accid;
};

