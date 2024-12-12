#include <iostream>
#include <asio.hpp>
#include <memory>
#include <map>
#include <functional>

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

    tcp::acceptor acceptor_;
public:
    static std::map<std::string, std::function<void(void)>> get;
    static std::map<std::string, std::function<void(void)>> post;
    static std::map<std::string, std::function<void(void)>> del;
    static std::map<std::string, std::function<void(void)>> update;

public:
    Server(asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

    // Get 함수 선언
    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    static void Get(const std::string path, const T&& callback) {
        get.emplace(path, std::function<void(void)>(callback));
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    static void Post(std::string path, T callback) {
        post[path] = callback;
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    static void Delete(std::string path, T callback) {
        del[path] = callback;
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    static void Update(std::string path, T callback) {
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

std::map<std::string, std::function<void(void)>> Server::get;
std::map<std::string, std::function<void(void)>> Server::post;
std::map<std::string, std::function<void(void)>> Server::update;
std::map<std::string, std::function<void(void)>> Server::del;

int main() {

    Server::Get("/", []() {
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
