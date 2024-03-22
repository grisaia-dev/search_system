#pragma once

//#include "../../includes/db/database.hpp"
#include "boost/asio/steady_timer.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
//#include <boost/beast/version.hpp>
//#include <boost/asio.hpp>
#include <iostream>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class server {
public:
    server(asio::io_context& io_service, unsigned short port);

private:
    void do_accept();

private:
    tcp::acceptor m_acceptor;
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket);

    void run();

private:
    void wait_for_request();
    void parse_request();
    void response_get();
    void response_post(); // TODO: Сделать ответ от сервера при запросе в поисковой строке
    void send_response();
    void check_time();

private:
    tcp::socket m_socket;
    beast::flat_buffer m_buffer{8192};
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;
    asio::steady_timer timer{m_socket.get_executor(), std::chrono::milliseconds(60)};
};