#pragma once

#include <pqxx/pqxx>
#include <iostream>

class db {
public:
    db(std::string& connect_to);
    ~db();

    void create_structure();



private: // Exception
    class Exception_notValid : public pqxx::broken_connection {
	public:
		using pqxx::broken_connection::broken_connection;
	};

private:
    pqxx::connection* m_connection = nullptr;
};