// ChimeraTCPServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <boost/asio.hpp>
#include <iostream>
#include "BoostAsyncTCPServer.h"

int main()
{
    std::cout << "Hello World!\n";
    //boost::asio::io_context io_context;
    //BoostAsyncTCPServer server(io_context);
    //Server server("127.0.0.1", 8888);

    try {
        Server server("127.0.0.1", 8888);
        std::cout << "Server running in another thread. Press Enter to stop...\n";
        std::cin.get(); // Wait for user input to stop the server
        //server.stop();  // Stop the server when the user presses Enter
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    //server.run();
    //io_context.run();
}
