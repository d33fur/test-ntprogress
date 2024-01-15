#include "session.h"

int main() {
  try {
    auto const address = net::ip::make_address("0.0.0.0");
    unsigned short port = static_cast<unsigned short>(std::atoi("8001"));
    
    net::io_context ioc{1};

    tcp::acceptor acceptor{ioc, {address, port}};

    tcp::socket socket{ioc};

    http_server(acceptor, socket);

    ioc.run();
  }
  catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}