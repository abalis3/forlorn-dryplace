#include "ServerSession.h"
#include "pbuf/generated/NetworkMessage.pb.h"

//#define SERVER_ADDR "127.0.0.1"
#define SERVER_ADDR "193.161.193.99"
#define SERVER_PORT 47411

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

ServerSession::ServerSession()
{
    connection = nullptr;
    callback = nullptr;
    retryNameRequest = false;
}

ServerSession::~ServerSession()
{
    if (connection != nullptr) {
        delete connection;
    }
}

void ServerSession::registerCallback(std::function<void(Event)> cb)
{
    callback = cb;
}

void ServerSession::open(std::string name)
{
    /* Create a connection if not yet done */
    if (connection == nullptr){

        ConnectionCallbacks cbs = {
            std::bind(&ServerSession::onConnectSuccess, this, _1),
            std::bind(&ServerSession::onConnectFail, this, _1),
            std::bind(&ServerSession::onConnectionLost, this, _1),
            std::bind(&ServerSession::onMsgReceived, this, _1, _2),
        };

        try {
            connection = new Connection(SERVER_ADDR, SERVER_PORT, 10, cbs);
            this->name = name;
        } catch (ConnectionException &exc) {
            if (callback != nullptr) {
                callback(Event::CONNECTION_LOST);
            }
        }
    } else {
        this->name = name;
        sendNameRequest();
    }
}

void ServerSession::poll(double secs)
{
    if (connection != nullptr) {
        connection->poll(secs);

        if (retryNameRequest) {
            retryNameRequest = false;
            sendNameRequest();
        }
    }
}

void ServerSession::sendNameRequest()
{
    pbuf::NetworkMessage msg;
    msg.set_namerequest(name);
    try {
        connection->sendNetworkMessage(msg);
    } catch (ConnectionException &exception) {
        retryNameRequest = true;
    }
}

void ServerSession::onConnectSuccess(Connection *conn)
{
    if (conn != connection) {
        /* This is probably bad... */
        return;
    }

    if (callback != nullptr) {
        sendNameRequest();
    }
}

void ServerSession::onConnectFail(Connection *conn)
{
    if (conn != connection) {
        /* This is probably bad... */
        return;
    }

    delete connection;
    connection = nullptr;
    if (callback != nullptr) {
        callback(Event::CONNECTION_LOST);
    }
}

void ServerSession::onConnectionLost(Connection *conn) {
    
    if (conn != connection) {
        /* This is probably bad... */
        return;
    }

    delete connection;
    connection = nullptr;
    if (callback != nullptr) {
        callback(Event::CONNECTION_LOST);
    }
}

void ServerSession::onMsgReceived(Connection *conn, pbuf::NetworkMessage msg) {
    switch(msg.type_case()) {
    case pbuf::NetworkMessage::kNameReply:
        if (msg.namereply()) {
            if (callback != nullptr) {
                callback(Event::NAME_ACCEPTED);
            }
        } else {
            if (callback != nullptr) {
                callback(Event::NAME_REJECTED);
            }
        }
        break;

    default:
        break;
    }
}