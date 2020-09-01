#include "Connection.h"
#include <unistd.h>
#include <iostream>

#define PORT 44444

static void onConnAccept(Connection *conn)
{
    std::cout << "Connection Received" << std::endl;
    //delete conn;
}

int main()
{
    Listener *listener;
    bool running = true;

    /* Start listenening for incoming connections to the server */
    try {
        listener = new Listener(PORT, onConnAccept);
    } catch (ListenerException &exp) {
        std::cout << "Failed to create listener on port " << PORT << std::endl;
        return 1;
    }

    std::cout << "Listening on port " << PORT << std::endl;
    
    while (running) {
        listener->poll();
    }

    std::cout << "Stopping listener and destroying server" << std::endl;
    delete listener;
    return 0; 
}