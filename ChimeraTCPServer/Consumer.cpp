#include "Consumer.h"
#include <iostream>
#include "BoostAsyncTCPServer.h"

void Consumer::consume()
{
    std::string message;
    while (true) {
        auto data = queue_.pop();
        message = data.msg;
        auto connection = std::static_pointer_cast<Session>(data.connection);
        // Process the message
        std::cout << "Consumer::consume: Processed message: " << message << std::endl;
        connection->do_write("Consumer::consume: I have recieved.");
    }
}
