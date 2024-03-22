#include "../includes/parser/parser.hpp"
#include "includes/httpserver.hpp"
#include <iostream>


int main(void) {
    INIP::Parser* parse_file = new INIP::Parser("config.ini");
    std::string s_db_connect;
    s_db_connect = "host=" + parse_file->get_value<std::string>("DB.host") + " " +
                   "port=" + parse_file->get_value<std::string>("DB.port") + " " + 
                   "dbname=" + parse_file->get_value<std::string>("DB.dbname") + " " +
                   "user=" + parse_file->get_value<std::string>("DB.user") + " " +
                   "password=" + parse_file->get_value<std::string>("DB.password");
    unsigned short port = parse_file->get_value<unsigned short>("SERVER.port");
    delete parse_file;


    asio::io_context io_service;
    server* serv = new server(io_service, port);
    io_service.run();
    delete serv;


    return EXIT_SUCCESS;
}