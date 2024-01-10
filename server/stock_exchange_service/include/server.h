#ifndef SERVER_H
#define SERVER_H

#include "session.h"

class MyServer {
public:
    MyServer(boost::asio::io_context& io_context, std::uint16_t port);

private:
    void accept();

    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

#endif // SERVER_H