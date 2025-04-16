#include "SessionList.h"

#include "ServerLog.h"

/* For std::bind _1, _2 ... */
using namespace std::placeholders;

Session::Session()
{
    conn = nullptr;
    name = nullptr;
    markedForDeletion = false;
}

Session::~Session()
{
    if (conn != nullptr) {
        delete conn;
    }
    if (name != nullptr) {
        delete name;
    }
}

std::string * Session::getName()
{
    return name;
}

Connection * Session::getConnection()
{
    return conn;
}

void Session::setName(std::string name)
{
    this->name = new std::string(name);
}

void Session::setConnection(Connection *conn)
{
    this->conn = conn;
}

Session * Session::getNext()
{
    return next;
}

void Session::onMsgRecv(Connection *unused, pbuf::NetworkMessage msg) {
    switch(msg.type_case()) {

    case pbuf::NetworkMessage::kNameRequest:
        pbuf::NetworkMessage reply;
        bool nameSuccess = canAcceptName(this, msg.namerequest());
        if (nameSuccess) {
            if (getName() == nullptr) {
                Logger::log() << "Accepted name '" << msg.namerequest() << "' for session with " <<
                        conn->getPeerIp() << ":" << conn->getPeerPort();
            } else {
                Logger::log() << "Updated name to '" << msg.namerequest() << "' for session with " <<
                        conn->getPeerIp() << ":" << conn->getPeerPort();
            }
            setName(msg.namerequest());
        } else {
            Logger::log() << "Already-taken name '" << msg.namerequest() << "' rejected for session with " <<
                    conn->getPeerIp() << ":" << conn->getPeerPort();
        }
        reply.set_namereply(nameSuccess);
        conn->sendNetworkMessage(reply);
        break;

    case pbuf::NetworkMessage::kJoinLobbyRequest:
        addToLobby(this, msg.joinlobbyrequest());
        break;

    default:
        break;
    }
}

void Session::onConnectionLost(Connection *unused) {
    Logger::log() << "Connection with " << conn->getPeerIp() << ":" << conn->getPeerPort() << " terminated";
    markedForDeletion = true;
}

void Session::onConnectionSuspended(Connection *unused) {
    Logger::log() << "SUSPENDED connection with " << conn->getPeerIp() << ":" << conn->getPeerPort();
}

void Session::onConnectionResumed(Connection *unused) {
    Logger::log() << "RESUMED connection with " << conn->getPeerIp() << ":" << conn->getPeerPort();
}

SessionList::SessionList()
{
    listHead = nullptr;
}

SessionList::~SessionList()
{
    /* Free any remaining sessions in the list */
    Session *temp;
    while(listHead != nullptr) {
        temp = listHead->next;
        delete listHead;
        listHead = temp;
    }
}

Session * SessionList::generateSession(Connection *conn, SessionNameCheckFnPtr nameCheckFn,
        SessionAddToLobyFnPtr addToLobbyFn)
{
    Session *sess = new Session();

    sess->next = listHead;
    sess->prev = nullptr;

    if (listHead != nullptr) {
        listHead->prev = sess;
    }

    listHead = sess;

    conn->setOnMsgReceivedCallback(std::bind(&Session::onMsgRecv, sess, _1, _2));
    conn->setOnConnectionLostCallback(std::bind(&Session::onConnectionLost, sess, _1));
    conn->setOnConnectionSuspendedCallback(std::bind(&Session::onConnectionSuspended, sess, _1));
    conn->setOnConnectionResumedCallback(std::bind(&Session::onConnectionResumed, sess, _1));
    sess->setConnection(conn);

    sess->canAcceptName = nameCheckFn;
    sess->addToLobby = addToLobbyFn;

    return sess;
}

void SessionList::destroySession(Session *sess)
{
    if (sess->next != nullptr) {
        sess->next->prev = sess->prev;
    }

    if (sess->prev != nullptr) {
        sess->prev->next = sess->next;
    } else {
        assert(listHead == sess);
        listHead = sess->next;
    }

    delete sess;
}

Session * SessionList::findByConnection(Connection * conn)
{
    Session *temp = listHead;
    while(temp != nullptr) {
        if (temp->conn == conn) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

Session * SessionList::findByName(std::string name)
{
    Session *temp = listHead;
    while (temp != nullptr) {
        if (temp->name != nullptr && *(temp->name) == name) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}

Session * SessionList::getFirst()
{
    return listHead;
}

void SessionList::pollSessions(double elapsedSecs)
{
    Session *sess = listHead, *prev;
    while (sess != nullptr) {
        Connection *conn = sess->getConnection();
        prev = sess;
        sess = sess->getNext() /* Advance sess before polling, since it could get removed */
        if (conn != nullptr) {
            conn->poll(elapsedSecs);
        }
        if (prev->markedForDeletion) {
            destroySession(prev);
        }
    }
}
