#include "server.h"

MyServer::MyServer(asio::io_context& io_context, std::uint16_t port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context) {
    accept();
}

void MyServer::accept() {
    std::cout << "LogFunc: accept()" << std::endl;
    acceptor_.async_accept(socket_, [this](boost::beast::error_code ec) {
        if (!ec) {
            std::make_shared<MySession>(std::move(socket_))->start();
        }
        
        accept();
    });
}
