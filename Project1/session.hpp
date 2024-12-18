#pragma once
#include <asio.hpp>
#include <functional>
#include "header.hpp"
#include "router.hpp"

using namespace asio::ip;

class Session : public std::enable_shared_from_this<Session> {
   
    enum { MAX_LEN = 1024 };
    char data_[MAX_LEN];

    asio::io_context& io_context_;
    tcp::socket socket_;

public:
    explicit Session(tcp::socket socket, asio::io_context& io_context)
        : socket_(std::move(socket)), io_context_(io_context) {}

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

                    std::function<std::string(void)> behave;
                    if (v[METHOD] == "GET") {
                        behave = router->get[v[PATH]];
                    }
                    else if (v[METHOD] == "POST") {
                        behave = router->post[v[PATH]];
                    }
                    else if (v[METHOD] == "UPDATE") {
                        behave = router->update[v[PATH]];
                    }
                    else if (v[METHOD] == "DELETE") {
                        auto behave = router->del[v[PATH]];
                    }
                    behave();
                    do_write(10);
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