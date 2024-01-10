#include "server.h"

int main() {
    try {
        asio::io_context io_context;
        MyServer server(io_context, 8001);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}