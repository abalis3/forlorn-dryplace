#include <unistd.h>
#include <iostream>
#include <chrono>

#include "SessionList.h"

#define PORT 44444

SessionList *sessions;

static void onMsgRecv(Connection *conn, pbuf::NetworkMessage msg) {
    if (msg.type_case() == pbuf::NetworkMessage::kNameRequest) {
        pbuf::NetworkMessage reply;
        Session *sess = sessions->findByName(msg.namerequest());
        bool nameSuccess = ((sess == nullptr) || (sess->getConnection() == conn));
        if (nameSuccess) {
            if (sess == nullptr) {
                sess = sessions->findByConnection(conn);
            }
            if (sess->getName() == nullptr) {
                std::cout << "Accepted name '" << msg.namerequest() << "' for session with " <<
                        conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
            } else {
                std::cout << "Updated name to '" << msg.namerequest() << "' for session with " <<
                        conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
            }
            sess->setName(msg.namerequest());
        } else {
            std::cout << "Already-taken name '" << msg.namerequest() << "' rejected for session with " <<
                    conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
        }
        reply.set_namereply(nameSuccess);
        conn->sendNetworkMessage(reply);
    }
}

static void onConnectionLost(Connection *conn) {
    Session *sess = sessions->findByConnection(conn);
    if (sess != nullptr) {
        std::cout << "Connection with " << conn->getPeerIp() << ":" << conn->getPeerPort() << " terminated" << std::endl;
        sessions->destroySession(sess);
    }
}

static void onConnectionSuspended(Connection *conn) {
    std::cout << "SUSPENDED connection with " << conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
}

static void onConnectionResumed(Connection *conn) {
    std::cout << "RESUMED connection with " << conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
}

static void onConnAccept(Connection *conn)
{
    conn->setOnMsgReceivedCallback(onMsgRecv);
    conn->setOnConnectionLostCallback(onConnectionLost);
    conn->setOnConnectionSuspendedCallback(onConnectionSuspended);
    conn->setOnConnectionResumedCallback(onConnectionResumed);
    Session *sess = sessions->generateSession();
    sess->setConnection(conn);

    std::cout << "Connection received from " << conn->getPeerIp() << ":" << conn->getPeerPort() << std::endl;
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
    
    sessions = new SessionList();

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running) {

        /* Update the elapsed time for this loop for polling */
        auto elapsedTime = std::chrono::high_resolution_clock::now() - lastTime;
        double elapsedSecs = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime).count() / 1000000000.0d;
        lastTime += std::chrono::nanoseconds((uint64_t) (1000000000 * elapsedSecs));

        listener->poll();
        Session *sess = sessions->getFirst();
        if (sess == nullptr) {
            usleep(50000); /* If no sessions currently active, sleep for .05 seconds to conserve CPU */
        }
        while (sess != nullptr) {
            Connection *conn = sess->getConnection();
            /* Once we poll the conn, we can't use sess anymore (could have been destroyed), so last use of it is here */
            sess = sess->getNext();
            if (conn != nullptr) {
                conn->poll(elapsedSecs);
            }
        }
    }

    std::cout << "Stopping listener and destroying server" << std::endl;
    delete sessions;
    delete listener;
    return 0; 
}