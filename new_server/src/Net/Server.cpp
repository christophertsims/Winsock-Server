#include "./Server.h"

Server::Server()
{

}

int Server::initialize(int pPortNo)
{

	printf("Server initializing... \n");

	printf("Connecting to database... \n");
	//db_con.connect();


	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		perror("WSAStarup could not be initialized.");
		return -1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if ((getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &serv_info)) != 0)
	{
		perror("Failed to create address info");
		return -1;
	}

	SOCKET server_sock;

	if ((server_sock = socket(AF_INET, serv_info->ai_socktype, serv_info->ai_protocol)) == INVALID_SOCKET)
	{
		perror("Could not create server socket");
		return -1;
	};

	// int yes = 1;
	// if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes,
	//                sizeof(int)) == -1)
	// {
	//     perror("Port was in use and could not grab it using setsockopt");
	//     return -1;
	// }

	if ((bind(server_sock, serv_info->ai_addr, (int)serv_info->ai_addrlen)) == SOCKET_ERROR)
	{
		perror("Failed to bind server to port");
		return -1;
	};

	add_connection(server_sock);

	freeaddrinfo(serv_info);

	std::cout << "Done initializing the server \n";

	return 0;
}

int Server::start()
{
	printf("Server Started \n");

	// Make the server listen
	running = true;
	reading = true;

	server_threads.push_back(std::thread(&Server::start_listen, this));

	//for (int i = 0; i < reader_thread_num; i++)
	//{
		//server_threads.push_back(std::thread(&Server::read, this));
	//}

	return 0;
}

int Server::stop()
{
	listening = false;
	reading = false;

	read_from_queue.notify_all();

	// join all threads
	for (std::thread& th : server_threads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	if ((closesocket(fdArray[0].fd)) == -1)
	{
		perror("Failed to close server socket");
		listening = false;
		return -1;
	};

	running = false;
	printf("Server stoped \n");
	return 0;
}

int Server::start_listen()
{
	if ((listen(fdArray[0].fd, SOMAXCONN)) == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(fdArray[0].fd);
		WSACleanup();
		listening = false;
	}
	else
	{
		listening = true;
	}

	while (listening)
	{
		if ((WSAPoll(&fdArray[0], fdArray.size(), 1)) > 0)
		{
			if (fdArray[0].revents & POLLERR) // Checks for error
			{
				printf("Listening socket poll error \n");
				continue;
			}

			if (fdArray[0].revents & POLLRDNORM)
			{
				addr_size = sizeof remote_addr;
				connecting_sock = accept(fdArray[0].fd, NULL, NULL);

				if (connecting_sock == INVALID_SOCKET)
				{
					perror("Failed to accept the new socket.");
					return -1;
				}
				else // On successfull accept, add its info to the connections array
				{
					printf("New connection \n");
					add_connection(connecting_sock);
				}
			}

			for (int i = 1; i < fdArray.size(); i++)
			{
				if (fdArray[i].revents & POLLHUP) // Checks for error
				{
					std::cout << "Client socket discnnected. (Found while polling) EVENT: " << fdArray[i].revents << std::endl;
					closesocket(fdArray[i].fd);
					remove_connection(i);
					continue;
				}

				if (fdArray[i].revents & POLLERR) // Checks for error
				{
					std::cout << "Client socket poll error. EVENT: " << fdArray[i].revents << std::endl;
					closesocket(fdArray[i].fd);
					remove_connection(i);
					continue;
				}

				if (fdArray[i].revents & POLLRDNORM) // Normal data can be read
				{

					printf("bytes recieved... \n");

					size_t result = fdData.at(&fdArray[i].fd).recieve();

					if (result == 0) // If connection was lost
					{
						printf("Client disconected \n");
						closesocket(fdArray[i].fd);
						remove_connection(i);
						continue;
					}

					if (result == SOCKET_ERROR) // If error occurred on socket
					{
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK)
						{
							closesocket(fdArray[i].fd);
							remove_connection(i);
							continue;
						}
					}

					if (result > 0)
					{
						if (result >= 2 || length > 0) // Should probably check that result is > 4 bytes before reading?
						{
							const int8_t* bytes = fdData.at(&fdArray[i].fd).get_buffer();
							process(bytes, result, &fdData.at(&fdArray[i].fd));
						}
					}
				}
			}
		}
	}

	printf("Listening thread coming to an end... \n");

	return 0;
}


void Server::process(const int8_t* bytes, size_t available, SockData* pSockData)
{
	if (pos == 0 && length == 0)
	{
		InPacket packet(bytes, available);
		length = packet.read_short();

		std::cout << "incoming packet length " << length << std::endl;

		// Move the bytes by 2 (The header is 2 bytes)
		bytes = bytes + 2;
		available -= 2;
	}

	// Determine how much we can write
	size_t to_write = length - pos;

	if (to_write > available)
		to_write = available;

	memcpy(buffer + pos, bytes, to_write);
	pos += to_write;

	// Check if the current packet has been fully processed
	if (pos >= length)
	{
		// handle packet
		packetSwitch.forward(bytes, length, pSockData);

		pos = 0;
		length = 0;

		// Check if there are bytes from some other packet in the buffer
		size_t remaining = available - to_write;

		if (remaining >= 2)
		{
			// More packets are available, so we start over
			process(bytes + to_write, remaining, pSockData);
		}
		else {
			std::cout << "Done processing" << std::endl;
		}
	}
}

void Server::read()
{

	while (reading)
	{
		std::unique_lock<std::mutex> lck(packet_queue_lock);

		while (packet_queue.size() == 0 && reading)
		{
			read_from_queue.wait(lck);
		}

		// CRITICAL
		if (packet_queue.size() > 0 && reading)
		{
			//packet_handler.read();
		}
		// CRITICAL

		lck.unlock();

		write_to_queue.notify_one();
	}

	printf("Reading thread coming to an end...\n");
}

void Server::add_connection(SOCKET pNewConnection)
{

	pollfd newfd;
	newfd.fd = pNewConnection;
	newfd.events = POLLRDNORM;
	newfd.revents = 0;

	fdArray.push_back(newfd);

	SockData new_sock_data(&fdArray.back());

	fdData.insert(std::make_pair(&fdArray.back().fd, new_sock_data));
}

void Server::remove_connection(int pSocketIndex)
{
	printf("Removing connection... \n");

	fdData.erase(&fdArray[pSocketIndex].fd);

	std::vector<WSAPOLLFD>::iterator it = fdArray.begin();
	fdArray.erase(it + pSocketIndex);
}

void Server::add_online_user(std::string pUserName)
{
	online_users[pUserName] = 0;
}

Poco::Data::RecordSet Server::db_communicate(std::string pStatement)
{
	return db.execute_statement(pStatement);
}

int Server::connection_count()
{
	return connections.size();
}

Poco::Data::Session& Server::get_database_connection()
{
	return db.get_session();
}