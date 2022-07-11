#include "SockData.h"

// PUBLIC

SockData::SockData(pollfd* pSocket)
	:
	socket(pSocket),
	player(&socket->fd)
{

}

size_t SockData::recieve()
{
	int recv_size = 0;

	if ((recv_size = recv(socket->fd, (char*)_buffer, 512, 0)) == SOCKET_ERROR)
	{
		puts("[ERROR] : Failed to recv \n");
	}

	if (recv_size == 0)
	{
		printf("Server closed its connection with the client \n");
		closesocket(socket->fd);
	}

	return recv_size;
}

bool SockData::dispatch(int8_t* bytes, size_t length)
{

	int total = 0;
	int len = length;
	int bytes_left = length;
	int n;

	std::cout << "Sending : " << length << " bytes." << std::endl;

	while (total < len)
	{
		n = send(socket->fd, (char*)bytes + total, static_cast<int>(bytes_left), 0);
		if (n == -1) { break; }
		total += n;
		bytes_left -= n;
	}

	len = total;

	return 0;
}

Player* SockData::get_player()
{
	return &player;
}

const int8_t* SockData::get_buffer()
{
	return _buffer;
}


//PRIVATE