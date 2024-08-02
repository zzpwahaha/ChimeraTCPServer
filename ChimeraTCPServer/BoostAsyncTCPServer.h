#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <functional>
#include <memory>
#include <iostream>
#include <ThreadsafeQueue.h>
#include <Consumer.h>

using boost::asio::ip::tcp;

//class TCPConnection
//    : public std::enable_shared_from_this<TCPConnection> 
//{
//public:
//    typedef std::shared_ptr<TCPConnection> pointer;
//
//    static pointer create(boost::asio::io_context& io_context)
//    {
//        return pointer(new TCPConnection(io_context));
//    }
//
//    tcp::socket& socket()
//    {
//        return socket_;
//    }
//    void start()
//    {
//        message_ = "Hello from the server!\n";
//        
//        std::cout << "TCPConnection::start " << message_ << std::endl;
//        
//        boost::asio::async_write(socket_, boost::asio::buffer(message_),
//            boost::bind(&TCPConnection::handle_write, shared_from_this(),
//                boost::asio::placeholders::error,
//                boost::asio::placeholders::bytes_transferred));
//        
//        std::cout << "TCPConnection::start after async_write " << std::endl;
//    }
//
//private:
//    TCPConnection(boost::asio::io_context& io_context)
//        : socket_(io_context)
//    {
//    }
//
//    void handle_write(const boost::system::error_code& /*error*/,
//        size_t /*bytes_transferred*/)
//    {
//    }
//
//    tcp::socket socket_;
//    std::string message_;
//};
//
//class BoostAsyncTCPServer
//{
//public:
//    BoostAsyncTCPServer(boost::asio::io_context& io_context)
//        : io_context_(io_context),
//        acceptor_(io_context, tcp::endpoint(tcp::v4()/*boost::asio::ip::make_address("127.0.0.10")*/, 8888))
//    {
//        std::cout << "BoostAsyncTCPServer" << std::endl;
//        
//        start_accept();
//    }
//
//private:
//    void start_accept()
//    {
//        TCPConnection::pointer new_connection =
//            TCPConnection::create(io_context_);
//
//        std::cout << "BoostAsyncTCPServer::start_accept" << std::endl;
//        
//        acceptor_.async_accept(new_connection->socket(),
//            boost::bind(&BoostAsyncTCPServer::handle_accept, this, new_connection,
//                boost::asio::placeholders::error));
//    }
//
//    void handle_accept(TCPConnection::pointer new_connection,
//        const boost::system::error_code& error)
//    {
//        std::cout << "BoostAsyncTCPServer::handle_accept" << std::endl;
//
//        if (!error)
//        {
//            new_connection->start();
//        }
//
//        start_accept();
//    }
//
//private:
//    tcp::acceptor acceptor_;
//    boost::asio::io_context& io_context_;
//
//};



class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, ThreadsafeQueue<TCPMessageTyep>& queue) :
        socket_(std::move(socket)),
        queue_(queue)
    {}
    ~Session() {
        std::cout << "Session::~Session" << std::endl;
    }
    void start() {
        std::cout << "Session::start" << std::endl;
        do_read();
    }

    void do_write(const std::string& msg) {
        std::cout << "Session::do_write " << msg << std::this_thread::get_id() << std::endl;
        auto self(shared_from_this());
        auto buffer = std::make_shared<std::string>(msg);
        boost::asio::async_write(socket_, boost::asio::buffer(*buffer),
            [this, self, buffer](boost::system::error_code ec, std::size_t /*length*/) {
                // The lambda handler captures the std::shared_ptr by value, 
                // extending the lifetime of the buffer until the asynchronous operation completes (handler get called).
                if (!ec) {
                    std::cout << "Session::do_write" << ec.message() << std::this_thread::get_id() <<std::endl;
                    //do_read();
                }
                else {
                    std::cout << "Session::do_write" << ec.message() << std::endl;
                }
            });
    }

private:
    void do_read() {
        std::cout << "Session::do_read" << std::this_thread::get_id() << std::endl;
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    queue_.push({ self, std::string(data_.begin(), data_.begin() + length) });
                    //do_write(length);
                    do_read();
                }
                else {
                    std::cout << "Session::do_read" << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    std::array<char, 1024> data_;
    ThreadsafeQueue<TCPMessageTyep>& queue_;
};

class Server {
public:
    Server(const std::string& address, short port) : 
        acceptor_(io_context_, tcp::endpoint(boost::asio::ip::make_address(address), port)),
        consumer_(queue_)
    {
        server_thread_ = boost::thread([this]() { io_context_.run(); });
        do_accept();
        consumer_.start();
    }
    ~Server() {
        io_context_.stop();
        if (server_thread_.joinable()) {
            std::cout << "Server::~Server" << std::endl;
            server_thread_.join();
        }
    }
private:
    void do_accept() {
        std::cout << "Server::do_accept" << std::endl;
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket), queue_)->start();
                }
                do_accept();
            });
    }
    boost::asio::io_context io_context_;
    tcp::acceptor acceptor_;
    boost::thread server_thread_;
    ThreadsafeQueue<TCPMessageTyep> queue_;
    Consumer consumer_;
};