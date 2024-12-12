#include "header.hpp"

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
                    //request handler
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