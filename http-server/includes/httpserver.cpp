#include "httpserver.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "../../includes/helper/help.hpp"

server::server(asio::io_context& io_service, unsigned short port) : m_acceptor(io_service, tcp::endpoint(tcp::v4(), port)) {
    std::cout << HIT << "Starting server..." << std::endl;
    // now we call do_accept() where we wait for clients
    do_accept();
    std::cout << GOOD << "Server is running!" << std::endl;
}

void server::do_accept() {
    // this is an async accept which means the lambda function is 
    // executed, when a client connects
    m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::cout << HIT << "Client: " << socket.remote_endpoint().address().to_string()
                      << ":" << socket.remote_endpoint().port() << std::endl;
            // create a session where we immediately call the run function
            std::make_shared<Session>(std::move(socket))->run();
        } else {
            std::cout << ERROR << ec.message() << std::endl;
        }
        do_accept();
    });
}

// ------------- SESSION FOR CLIENT ----------------

Session::Session(tcp::socket socket) : m_socket(std::move(socket)) { }

void Session::run() {
    wait_for_request();
    check_time();
}

void Session::wait_for_request() {
    auto self = shared_from_this();
    http::async_read(m_socket, m_buffer, m_request, [self](beast::error_code ec, std::size_t bytes_transferred){
        if (!ec) {
            boost::ignore_unused(bytes_transferred);
            self->parse_request();
        } else
            std::cout << ERROR << ec.message() << std::endl;
    });

}

void Session::parse_request() {
    m_response.version(m_request.version());
    m_response.keep_alive(false);

    switch (m_request.method()) {
        case http::verb::get:
            m_response.result(http::status::ok);
            m_response.set(http::field::server, "localhost");
            response_get();
            break;
        //case http::verb::post: сделать ответ при запросе
            //break;
        default:
            m_response.result(http::status::bad_request);
            m_response.set(http::field::server, "localhost");
            beast::ostream(m_response.body()) << "Invalid request method \'" << std::string(m_request.method_string()) << "\'";
            break;
    }
    send_response();
}

void Session::response_get() {
    if (m_request.target() == "/") {
        m_response.set(http::field::content_type, "text/html");
        beast::ostream(m_response.body())
             << "<html>\n"
			    "<head><meta charset=\"UTF-8\"><title>Search Engine</title></head>\n"
			    "<body>\n"
			    "<h1>Search Engine</h1>\n"
			    "<p>Welcome!<p>\n"
			    "<form action=\"/\" method=\"post\">\n"
			    "    <label for=\"search\">Search:</label><br>\n"
			    "    <input type=\"text\" id=\"search\" name=\"search\"><br>\n"
			    "    <input type=\"submit\" value=\"Search\">\n"
			    "</form>\n"
			    "</body>\n"
			    "</html>\n";
    } else {
        m_response.result(http::status::not_found);
		m_response.set(http::field::content_type, "text/plain");
		beast::ostream(m_response.body()) << "File not found\r\n";
    }
}

void Session::send_response() {
    auto self = shared_from_this();

	m_response.content_length(m_response.body().size());

	http::async_write(
		m_socket,
		m_response,
		[self](beast::error_code ec, std::size_t)
		{
			self->m_socket.shutdown(tcp::socket::shutdown_send, ec);
			self->timer.cancel();
		});
}

void Session::check_time() {
    auto self = shared_from_this();
    timer.async_wait([self](boost::system::error_code ec) {
        if (!ec) {
            std::cout << HIT << "Connection refused!" << std::endl;
            self->m_socket.close();
        } else
            std::cout << ERROR << "Timer canceled" << std::endl;
    });
}