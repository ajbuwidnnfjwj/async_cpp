#pragma once
#include <asio.hpp>
#include <iostream>
#include <fstream>

#include "router.hpp"
#include "session.hpp"

using asio::ip::tcp;

class Server {
    tcp::acceptor acceptor_;
    asio::io_context& io_context_;
public:
    Server(asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), io_context_(io_context) {
        do_accept();
    }

    static std::string&& get_html_file(std::string path) {
        std::ifstream html(path);

        return std::move(std::string((std::istreambuf_iterator<char>(html)),
            std::istreambuf_iterator<char>()));
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), io_context_)->start();
                }
                else {
                    std::cerr << "Accept error: " << ec.message() << "\n";
                }
                do_accept(); // 다음 클라이언트를 기다림
            });
    }
};