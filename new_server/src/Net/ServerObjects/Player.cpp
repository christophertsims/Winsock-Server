#include "Player.h"

Player::Player(SOCKET* pSocket)
{
	socket = pSocket;
}

void Player::set_username(std::string pUserName)
{
	user_name = pUserName;
}

void Player::set_id(int pId)
{
	accid = pId;
}