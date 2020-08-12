#ifndef FD__CONNECTION_H
#define FD__CONNECTION_H

/* Macros to define what OS we are building on */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   #define COMPILING_ON_WINDOWS 1
#elif __APPLE__
   #define COMPILING_ON_OSX 1
#elif __linux__
   #define COMPILING_ON_LINUX 1
#endif

#include <string>

#if COMPILING_ON_WINDOWS
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

/* 
 * Class representing a TCP socket connection. Essentially an abstraction layer to provide
 * multi-platform basic socket functionality for use by both the server and client.
 * Also encapsulates a paradigm for connection keep-alive wherein the socket sends a message
 * that expects a response at a configured interval. If the response is not received during
 * said interval, the connection is marked suspended. The connection is not fully disconnected
 * until either a second configured interval has passed or the OS implementation times out
 * after its alloted number of TCP retransmits. The Connection also supports the ability to
 * configure callbacks for connection status updates.
 */
class Connection {
 public:

    /* Initialize/start the socket subsystem - Only actually does anything on windows system */
    static void init();

    /* Uninit/stop the socket subsystem - Only actually does anything on windows system */
    static void quit();

    /* Constructor - Supply destination (peer) host/ip and port to connect to */
    Connection(std::string host, uint16_t port);

    /* Destructor to clean up memory used */
    ~Connection();

 private:

    /* The states this connection may be in */
    enum class State {
        DISCONNECTED,   /* No socket file descriptor - socket completely closed */
        SUSPENDED,      /* No socket error (yet) but inactivity / disruption detected */
        ACTIVE          /* Socket open, no disruption to connection checks */
    };

    /* Holds the current state of this connection */
    State currentState;

    /* Holds the socket handle - Platform specific */
#if COMPILING_ON_WINDOWS
    SOCKET sock;
#else
    int sock;
#endif

};

#endif