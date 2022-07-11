#include "LoginPacketHandlers.h"

#include "../../Server.h"

using namespace Poco::Data::Keywords;

void LoginRequestHandler::handle(InPacket pPacket, SockData* pSockData)
{
	printf("Login request recieved \n");

	std::string username = pPacket.read_string();
	std::string password = pPacket.read_string();

	Poco::Data::Session& mysql_session = Server::get().get_database_connection();

	Poco::Data::Statement statement1(mysql_session);
	statement1 << "select username from accounts where username = '" << username + "' and password = '" << password << "'";

	if (statement1.execute() == 0)
	{
		printf("Bad login \n");

		OutPacket bad_login(-1);
		bad_login.write_int(-1);

		bad_login.dispatch(pSockData);

		return;
	}

	std::string rU_Name = "";
	int rAcc_id = 0;

	Poco::Data::Statement statement2(mysql_session);
	statement2 << "SELECT accid, username FROM accounts WHERE username = '" << username << "' and password = '" << password << "'", into(rAcc_id), into(rU_Name);

	if (statement2.execute() == 0)
	{
		printf("Bad login \n");

		OutPacket bad_login(-1);
		bad_login.write_int(-1);

		bad_login.dispatch(pSockData);

		return;
	}

	printf("Successfull login \n");

	Player* player = pSockData->get_player();

	player->set_id(rAcc_id);
	player->set_username(rU_Name);

	Server::get().add_online_user(rU_Name);

	std::vector<std::string> char_names;
	Poco::Data::Statement statement3(mysql_session);
	statement3 << "SELECT charname FROM characters WHERE accid = '" << rAcc_id << "'";
	statement3.execute();
	Poco::Data::RecordSet rs1(statement3);

	bool more = rs1.moveFirst();
	while (more)
	{
		char_names.push_back(rs1["charname"].convert<std::string>());
		more = rs1.moveNext();
	}

	OutPacket successfull_login_packet(1);

	successfull_login_packet.write_string(rU_Name);
	successfull_login_packet.write_short(char_names.size());

	for (int i = 0; i < char_names.size(); i++)
	{
		successfull_login_packet.write_string(char_names[i]);
	}

	successfull_login_packet.dispatch(pSockData);
}