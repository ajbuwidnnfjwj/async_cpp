#include <iostream>
#include <asio.hpp>
#include <string>

#include "server.hpp"

extern Router* router;


int main() {

    router->Get("/", []() -> std::string {
        std::string html = Server::get_html_file("index.html");
        std::string http_response = "HTTP/1.1 200 OK\r\n";
        http_response += "Content-Type: text/html\r\n";
        http_response += "Content-Length: " + std::to_string(html.size()) + "\r\n";
        http_response += "\r\n";
        http_response += html;
        return http_response;
        });

    try {
        asio::io_context io_context;
        Server server(io_context, 12345); // 포트 12345에서 서버 실행
        std::cout << "Echo server is running on port 12345...\n";
        io_context.run(); // I/O 작업 실행
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
