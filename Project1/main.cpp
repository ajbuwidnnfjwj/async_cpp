#include <iostream>
#include <asio.hpp>
#include <memory>
#include <map>
#include <functional>

#include "server.hpp"


int main() {

    Server::route->Get("/", []() {
        std::cout << "hi";
        });

    try {
        asio::io_context io_context;
        Server server(io_context, 12345); // ��Ʈ 12345���� ���� ����
        std::cout << "Echo server is running on port 12345...\n";
        io_context.run(); // I/O �۾� ����
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
