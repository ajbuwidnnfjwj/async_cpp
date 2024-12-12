#include <iostream>
#include <asio.hpp>
#include <memory>
#include <map>
#include <functional>

#include "header.hpp"
#include "router.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {

    tcp::socket socket_;
    enum { MAX_LEN = 1024 };
    char data_[MAX_LEN];

public:
    explicit Session(tcp::socket socket)
        : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(
            asio::buffer(data_, MAX_LEN),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::vector<std::string> v = parse(data_);
                    
                }
                else {
                    std::cerr << "Read error: " << ec.message() << "\n";
                }
            });
    }

    void do_write(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(
            socket_, asio::buffer(data_, length),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                }
                else {
                    std::cerr << "Write error: " << ec.message() << "\n";
                }
            });
    }

    void handle_request(const std::string& method, const std::string& path) {
        if (method == "GET") {
            auto behave = router::get[path];
            behave();
        }            
        else if (method == "POST") {
            auto behave = router::post[path];
            behave();
        }
        else if (method == "UPDATE") {
            auto behave = router::update[path];
            behave();
        }
        else if (method == "DELETE") {
            auto behave = router::del[path];
            behave();
        }
        else {
            //handle err
        }
    }
};

class Server {

    tcp::acceptor acceptor_;

public:
    Server(asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                else {
                    std::cerr << "Accept error: " << ec.message() << "\n";
                }
                do_accept(); // 다음 클라이언트를 기다림
            });
    }
};

std::map<std::string, std::function<void(void)>> router::get;
std::map<std::string, std::function<void(void)>> router::post;
std::map<std::string, std::function<void(void)>> router::del;
std::map<std::string, std::function<void(void)>> router::update;

int main() {

    router::Get("/", []() {
        std::cout << "hi";
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
