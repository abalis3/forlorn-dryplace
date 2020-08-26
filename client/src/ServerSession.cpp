#include "ServerSession.h"

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

ServerSession::ServerSession()
{
    connection = nullptr;
    callback = nullptr;
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
            .onConnectSuccess = std::bind(&ServerSession::onConnectSuccess, this, _1),
            .onConnectFail = std::bind(&ServerSession::onConnectFail, this, _1),
        };

        try {
            connection = new Connection("127.0.0.1", 44444, 10, cbs);
        } catch (ConnectionException &exc) {
            if (callback != nullptr) {
                callback(Event::CONNECTION_LOST);
            }
        }
    }
}

void ServerSession::poll(double secs)
{
    if (connection != nullptr) {
        connection->poll(secs);
    }
}

void ServerSession::onConnectSuccess(Connection *conn)
{
    if (conn != connection) {
        /* This is probably bad... */
        return;
    }

    if (callback != nullptr) {
        callback(Event::NAME_ACCEPTED);
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
