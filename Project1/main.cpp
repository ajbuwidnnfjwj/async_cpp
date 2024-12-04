#include <iostream>
#include <asio.hpp>
#include <memory>
#include <map>

#include "header.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

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
            asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::vector<std::string> v = parse(data_);
                    std::cout << v[method];
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
};

class Server {
    using CallBack = void(std::string);

    tcp::acceptor acceptor_;
public:
    static std::map<std::string, std::string> get;
    static std::map<std::string, std::string> post;
    static std::map<std::string, std::string> del;
    static std::map<std::string, std::string> update;

public:
    Server(asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

    template<class CallBack>
    static void Get(std::string path, CallBack callback) {
        get[path] = callback;
    }

    template<class CallBack>
    static void Post(std::string path, CallBack callback) {
        post[path] = callback;
    }

    template<class CallBack>
    static void Delete(std::string path, CallBack callback) {
        del[path] = callback;
    }

    template<class CallBack>
    static void Update(std::string path, CallBack callback) {
        update[path] = callback;
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

int main() {
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
