#include "./Database.h"


Database::Database() :
	mysql_session("MySQL", "user=root;password=slimex23;db=onlinedb;auto-reconnect=true")
{
	printf("database cerated!!!!!!!!!!!!!! \n");
	//execute_statement("SELECT * FROM ACCOUNTS");
}

Database::~Database()

{
	printf("Closing database connection. \n");
	//mysql_close(&mysql);
}

int Database::connect()
{
	printf("Successfully connected to database! \n");

	return 0;
}
Poco::Data::Session& Database::get_session()
{
	return mysql_session;
}

Poco::Data::RecordSet Database::execute_statement(std::string pStatement)
{
	std::cout << "Running SQL: " << pStatement << std::endl;

	Poco::Data::Statement statement1(mysql_session);
	statement1 << pStatement;
	statement1.execute();

	Poco::Data::RecordSet recordset1(statement1);

	return recordset1;
}