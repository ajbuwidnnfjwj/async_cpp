#include <asio.hpp>
#include <iostream>

#include "session.hpp"
#include "router.hpp"

using asio::ip::tcp;

class Server {
    tcp::acceptor acceptor_;
public:
    static router* route;
public:
    Server(asio::io_context& io_context, int port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

    ~Server() {
        delete route;
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

    void handle_request(const std::string& method, const std::string& path) {
        if (method == "GET") {
            auto behave = Server::route->get[path];
            behave();
        }
        else if (method == "POST") {
            auto behave = Server::route->post[path];
            behave();
        }
        else if (method == "UPDATE") {
            auto behave = Server::route->update[path];
            behave();
        }
        else if (method == "DELETE") {
            auto behave = Server::route->del[path];
            behave();
        }
        else {
            //handle err
        }
    }
};

router* Server::route = new router();