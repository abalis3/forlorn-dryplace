#ifndef FD__SESSIONLIST_H
#define FD__SESSIONLIST_H

#include <string>

#include "Connection.h"

/* Forward declarations for Session member pointer and function typedef */
class Session;
class SessionList;

/* Typedef for a function that will be called when checking if a name is available for a session */
typedef std::function<bool(Session*, const std::string&)> SessionNameCheckFnPtr;

/* Typedef for a function that will be called to request this session be added to a lobby */
typedef std::function<bool(Session*, const std::string&)> SessionAddToLobbyFnPtr;

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

    /* Handler for when a new message is received from the underlying connection */
    void onMsgRecv(Connection *conn, pbuf::NetworkMessage msg);

    /* Handler for when the underlying connection is lost */
    void onConnectionLost(Connection *conn);

    /* Handlers for when the underlying connection is suspended and resumed */
    void onConnectionSuspended(Connection *conn);
    void onConnectionResumed(Connection *conn);

    /* Pointer to a func to call to check if the given name can be used for this session */
    SessionNameCheckFnPtr canAcceptName;

    /* Pointer to a func to call to request that this session be added to a new or existing lobby */
    SessionAddToLobbyFnPtr addToLobby;

    /* Pointers for doubly-linked list of sessions */
    Session *next;
    Session *prev;

    /* Boolean representing when this Session should be deleted by the owning list */
    bool markedForDeletion;

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

    /*
     * Allocates a new session instance and adds it to the list of sessions, returning a reference.
     * Must supply a non-null connection to use for this session, and a callback function that will
     * check if a given name can be assigned to this session (the func should return true if the
     * name can be used. False otherwise). addToLobbyFn is the function that should be called to
     * request that this Session be added to a new or existing lobby (the func should return true
     * if the session was added to the Lobby successfully. False otherwise).
     */
    Session * generateSession(Connection *conn, SessionNameCheckFnPtr nameCheckFn,
            SessionAddToLobbyFnPtr addToLobbyFn);

    /* Removes a session from the session list and frees the associated memory */
    void destroySession(Session *sess);

    /* Find a session based on its Connection. If none found, returns nullptr */
    Session * findByConnection(Connection * conn);

    /* Find a session based on its registered name. If none found, returns nullptr */
    Session * findByName(std::string name);

    /* Returns the first Session in the list of sessions (the list head). nullptr if no sessions in list */
    Session * getFirst();

    /*
     * Poll each Session in this list (and its underlying connection). Note that Sessions may be
     * removed and/or deleted during this process
     */
    void pollSessions(double elapsedSecs);

 private:
    
    /* Points to the head of the doubly-linked list of sessions */
    Session *listHead;

};

#endif