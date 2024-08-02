#pragma once
#include <ThreadSafeQueue.h>
#include <iostream>

class Session;

struct TCPMessageTyep {
    std::shared_ptr<Session> connection;
    std::string msg;
};

class Consumer {
public:
    Consumer(ThreadsafeQueue<TCPMessageTyep>& queue) : queue_(queue) {}
    ~Consumer() {
        std::cout << "~Consumer" << std::endl;
        consumer_thread_.join();
    }

    void start() {
        consumer_thread_ = std::thread(&Consumer::consume, this);
        //consume();
    }

private:
    void consume();

    ThreadsafeQueue<TCPMessageTyep>& queue_;
    std::thread consumer_thread_;
};