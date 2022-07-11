#include <iostream>

#include "./Net/Server.h"

int main()
{
	printf("Program starting... \n");

	Server::get();
	Server::get().initialize(7785);
	Server::get().start();

	while (Server::get().running)
	{

	}

	return 0;
}