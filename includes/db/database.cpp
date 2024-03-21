#include "database.hpp"
#include "../helper/help.hpp"

db::db(std::string& connect_to) {
    try {
        std::cout << HIT << "Connecting to the database.." << std::endl;
        try {
            m_connection = new pqxx::connection(connect_to.c_str());
        } catch (const pqxx::broken_connection&) {}   
        if (m_connection != nullptr) {
            if (m_connection->is_open())
                std::cout << GOOD << "Connection completed!" << std::endl;
        } else
            throw Exception_notValid("It is impossible to connect to the database, check that the connection data is correct!");
    } catch (const Exception_notValid::broken_connection& ex) {
        std::cout << ERROR << ex.what() << std::endl;
    }
}
db::~db() {
    if (m_connection != nullptr) {
        if (m_connection->is_open()) {
            m_connection->close(); 
            std::cout << HIT << "Disconnecting from the database!" << std::endl;
        }
    }
    delete m_connection;
}

void db::create_structure() {
    if (m_connection != nullptr) {
        pqxx::work tx {*m_connection};
        tx.exec(tx.esc("CREATE SCHEMA IF NOT EXISTS database;"));
        tx.exec(tx.esc("CREATE TABLE IF NOT EXISTS database.words ("
                       "id SERIAL NOT NULL PRIMARY KEY, "
                       "word text NOT NULL);"
                      ));
        tx.exec(tx.esc("CREATE TABLE IF NOT EXISTS database.documents ("
                       "id SERIAL NOT NULL PRIMARY KEY, "
                       "document text NOT NULL);"
                      ));
        tx.exec(tx.esc("CREATE TABLE IF NOT EXISTS database.words_count ("
                       "id BIGSERIAL PRIMARY KEY, "
                       "words_id INTEGER REFERENCES database.words(id), "
                       "documents_id INTEGER REFERENCES database.documents(id), "
                       "count_word INTEGER);"
                      ));
        tx.commit();
        std::cout << GOOD << "The scheme has been created" << std::endl;
    }
}