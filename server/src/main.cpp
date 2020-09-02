#include <unistd.h>
#include <iostream>

#include "SessionList.h"

#define PORT 44444

SessionList *sessions;

static void onMsgRecv(Connection *conn, pbuf::NetworkMessage msg) {
    if (msg.type_case() == pbuf::NetworkMessage::kNameRequest) {
        pbuf::NetworkMessage reply;
        ///std::cout << "here1" << std::endl;
        Session *sess = sessions->findByName(msg.namerequest());
        ///std::cout << "here2" << std::endl;
        bool nameSuccess = ((sess == nullptr) || (sess->getConnection() == conn));
        if (nameSuccess) {
            ///std::cout << "here3" << std::endl;
            if (sess == nullptr) {
                ///std::cout << "here4" << std::endl;
                sess = sessions->findByConnection(conn);
                ///std::cout << "here5" << std::endl;
            }
            sess->setName(msg.namerequest());
            std::cout << "Name request approved for name '" << msg.namerequest() << "'" << std::endl;
        } else {
            std::cout << "Name request rejected with already taken name '"
                    << msg.namerequest() << "'" << std::endl;
        }
        reply.set_namereply(nameSuccess);
        conn->sendNetworkMessage(reply);
    }
}

static void onConnAccept(Connection *conn)
{
    conn->setOnMsgReceivedCallback(onMsgRecv);
    Session *sess = sessions->generateSession();
    sess->setConnection(conn);

    std::cout << "Connection Received" << std::endl;
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
            ///std::cout << "poll" << std::endl;
            Connection *conn = sess->getConnection();
            ///std::cout << "poll2" << std::endl;
            if (conn != nullptr) {
                ///std::cout << "poll3" << std::endl;
                conn->poll(1.0);
                ///std::cout << "poll4" << std::endl;
            }
            ///std::cout << "poll5" << std::endl;
            sess = sess->getNext();
        }
    }

    std::cout << "Stopping listener and destroying server" << std::endl;
    delete sessions;
    delete listener;
    return 0; 
}