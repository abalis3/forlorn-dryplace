#include <unistd.h>
#include <iostream>

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

static void onConnAccept(Connection *conn)
{
    conn->setOnMsgReceivedCallback(onMsgRecv);
    conn->setOnConnectionLostCallback(onConnectionLost);
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

    while (running) {
        listener->poll();
        Session *sess = sessions->getFirst();
        while (sess != nullptr) {
            Connection *conn = sess->getConnection();
            if (conn != nullptr) {
                conn->poll(1.0);
            }
            sess = sess->getNext();
        }
    }

    std::cout << "Stopping listener and destroying server" << std::endl;
    delete sessions;
    delete listener;
    return 0; 
}