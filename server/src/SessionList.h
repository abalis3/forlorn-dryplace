#ifndef FD__SESSIONLIST_H
#define FD__SESSIONLIST_H

#include <string>

#include "Connection.h"

/* Forward declaration for Session member pointer */
class SessionList;

/*
 * Represents a session in the session list, including the connection, the name registered,
 * and any other session-specific data
 */
class Session {
 public:
    
    /* Retrieve the name registered to the given session. Returns nullptr if no name registered */
    std::string * getName();

    /* Retrieve the connection associated with this session */
    Connection * getConnection();

    /* Sets the name registered to this session */
    void setName(std::string name);

    /* Sets the connection associated with this session */
    void setConnection(Connection *conn);

    /* Returns the next Session in the SessionList. nullptr if last Session */
    Session * getNext();

 private:
    
    /* Private constructor only for use by SessionList class */
    Session();

    /* 
     * Private destructor only used by SessionList class. Teardown connection and free name and other
     * allocated memory
     */
    ~Session();

    /* The connection currently registered for this Session */
    Connection *conn;

    /* The string representing the currently-registered name. nullptr if no name yet */
    std::string *name;

    /* Pointers for doubly-linked list of sessions */
    Session *next;
    Session *prev;

    /* Friend class declaration so SessionList can handle these as doubly-linked list entries */
    friend class SessionList;
};

/* 
 * This class represents a doubly-linked list of Sessions that are currently
 * being managed by the server. When a connection is first opened it is attached to a session
 * and placed into this list, even before obtaining a name to identify the session. The session
 * lives on in this list until it is completely lost or closed
 */
class SessionList {
 public:
    
    /* Constructor to initialize data */
    SessionList();

    /* Destructor to free any memory associated with the list */
    ~SessionList();

    /* Allocates a new session instance and adds it to the list of sessions, returning a reference */
    Session * generateSession();

    /* Removes a session from the session list and frees the associated memory */
    void destroySession(Session *sess);

    /* Find a session based on its Connection. If none found, returns nullptr */
    Session * findByConnection(Connection * conn);

    /* Find a session based on its registered name. If none found, returns nullptr */
    Session * findByName(std::string name);

    /* Returns the first Session in the list of sessions (the list head). nullptr if no sessions in list */
    Session * getFirst();

 private:
    
    /* Points to the head of the doubly-linked list of sessions */
    Session *listHead;

};

#endif