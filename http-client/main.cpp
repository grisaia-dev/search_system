//#include "includes/httpclient.hpp"
#include "../includes/parser/parser.hpp"
#include "../includes/db/database.hpp"
#include <iostream>

int main(void) {
    INIP::Parser* parse_file = new INIP::Parser("config.ini");
    std::string s_db_connect;
    s_db_connect = "host=" + parse_file->get_value<std::string>("DB.host") + " " +
                   "port=" + parse_file->get_value<std::string>("DB.port") + " " + 
                   "dbname=" + parse_file->get_value<std::string>("DB.dbname") + " " +
                   "user=" + parse_file->get_value<std::string>("DB.user") + " " +
                   "password=" + parse_file->get_value<std::string>("DB.password");
    delete parse_file;

    db* database = new db(s_db_connect);
    database->create_structure();

    delete database;  

    return EXIT_SUCCESS;
}