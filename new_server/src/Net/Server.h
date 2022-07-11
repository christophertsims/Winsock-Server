#pragma once
#pragma comment(lib, "Ws2_32.lib")



#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>   
#include <array>
#include <map>

#include "Poco/Data/RecordSet.h"

#include "../Patterns/Singleton.h"
#include "./SockData.h"
#include "./Packets/InPacket.h"
#include "./Packets/PacketSwitch.h"
#include "./Database.h"
#include "./ServerObjects/Player.h"

#define DEFAULT_PORT "7785"
#define BUFFER_SIZE 40

class Server : public Singleton<Server>
{
public:
	Server();
	int initialize(int pPortNo);
	int start();
	int stop();

	int start_listen();
	void read();
	void process(const int8_t* bytes, size_t available, SockData* pSockData);

	void add_connection(SOCKET pNewConnection);
	void remove_connection(int pSocketIndex);

	void add_online_user(std::string pUserName);

	Poco::Data::RecordSet db_communicate(std::string pStatement);

	int connection_count();

	Poco::Data::Session& get_database_connection();

	bool running = false;

private:
	Database db;

	char buffer[1024];

	std::vector<SockData> connections;
	std::map<SOCKET*, SockData> fdData;
	std::vector<WSAPOLLFD> fdArray;

	std::map<std::string, int> online_users;
	std::map<int, Player*> online_players;

	PacketSwitch packetSwitch;

	size_t length = 0;
	size_t pos = 0;

	// Packet storage
	std::mutex packet_queue_lock;
	std::condition_variable write_to_queue;
	std::condition_variable read_from_queue;

	std::vector<int> packet_queue;

	std::vector<std::thread> server_threads;
	SOCKET connecting_sock;

	int port_no;
	WSADATA wsa_data;
	struct addrinfo hints, * serv_info;
	struct sockaddr_storage remote_addr;
	socklen_t addr_size;

	bool listening = false;
	bool reading = false;

	int reader_thread_num = 3;
};