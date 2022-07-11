//#include "./Poco/Data/Session.h"
//#include <C:/dev/vcpkg/installed/x64-windows/include/mysql/mysql.h>

#include <iostream>
#include <string>

#include "./Poco/Data/Session.h"
#include "./Poco/Data/MySQL/Connector.h"
#include "Poco/Data/RecordSet.h"

class Wrapper {
public:
	Wrapper() {
		Poco::Data::MySQL::Connector::registerConnector();
	}
	~Wrapper() {
		Poco::Data::MySQL::Connector::unregisterConnector();
	}
};

class Database
{
public:
	Database();
	~Database();
	int connect();
	Poco::Data::Session& get_session();
	Poco::Data::RecordSet execute_statement(std::string pStatement);

private:
	Wrapper wrap;
	Poco::Data::Session mysql_session;
};