#include "SessionList.h"

Session::Session()
{
    conn = nullptr;
    name = nullptr;
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

Session * SessionList::generateSession()
{
    Session *sess = new Session();

    sess->next = listHead;
    sess->prev = nullptr;

    if (listHead != nullptr) {
        listHead->prev = sess;
    }

    listHead = sess;

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
