#ifndef FD__CONNECTION_H
#define FD__CONNECTION_H

/* Macros to define what OS we are building on */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   #define COMPILING_ON_WINDOWS 1
   #define COMPILING_ON_OSX 0
   #define COMPILING_ON_LINUX 0
#elif __APPLE__
   #define COMPILING_ON_WINDOWS 0
   #define COMPILING_ON_OSX 1
   #define COMPILING_ON_LINUX 0
#elif __linux__
   #define COMPILING_ON_WINDOWS 0
   #define COMPILING_ON_OSX 0
   #define COMPILING_ON_LINUX 1
#endif

#include <string>
#include <stdexcept>
#include <functional>

#include "pbuf/generated/NetworkMessage.pb.h"

#if COMPILING_ON_WINDOWS
    #define CloseWindow win_disabled_CloseWindow
    #include <winsock2.h>
    #undef CloseWindow
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#define RECV_BUFFER_SIZE 4096

/* Forward declaration to be used by ConnectionCallbacks */
class Connection;

/*
 * Structs representing connection callback functions. These must be set to either valid
 * function pointers or NULL if no callback is desired to listen for a specific event.
 */
struct ConnectionCallbacks {

    /* Called when a connection successfully connects */
    std::function<void(Connection*)> onConnectSuccess;

    /* 
     * Called when a connection has failed to connect.
     * The caller should destroy the failed connection as it can no longer be used.
     */
    std::function<void(Connection*)> onConnectFail;

    /* Called when the connection is lost completely or aborted after being opened successfully */
    std::function<void(Connection*)> onConnectionLost;

    /* 
     * Called when the connection moves from being ACTIVE to SUSPENDED. This means some network
     * instability has occurred but the connection is still potentially going to survive.
     */
    std::function<void(Connection*)> onConnectionSuspended;

    /*
     * Called when the connection moves from being SUSPENDED to ACTIVE. This means the connection
     * has recovered from instability and is again healthy.
     */
    std::function<void(Connection*)> onConnectionResumed;

    /*
     * Called when a NetworkMessage is received over the connection and has been parsed successfully
     * WARNING! No deleting the Connection with this callback in the function stack!
     */
    std::function<void(Connection*, pbuf::NetworkMessage)> onMsgReceived;
};

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

    /*
     * Constructor - Supply destination (peer) host/ip and port to connect to.
     * Also supply a timeout period. If timeout is reached before successful connection,
     * it will be aborted and treated as a connection error. Except for on an immediate failure,
     * the socket's connection status will be given by the callbacks onConnectSuccess/onConnectFail
     */
    Connection(std::string host, uint16_t port, double timeout, ConnectionCallbacks &callbacks);

    /* Destructor to clean up memory used */
    ~Connection();

    /* Send the given network message protobuf over the connection stream */
    void sendNetworkMessage(pbuf::NetworkMessage &msg);

    /* Poll method for the connection. This should be called regularly with the time since last call */
    void poll(double secs);

#if !COMPILING_ON_WINDOWS
    /* Get the peer ip address as a string for this connection */
    std::string getPeerIp();

    /* Get the peer port number as a number for this connection */
    uint16_t getPeerPort();
#endif

    /* Used to set the onConnectionLost callback function for the Connection */
    void setOnConnectionLostCallback(std::function<void(Connection*)> cb);

    /* Used to set the onConnectionSuspended callback function for the Connection */
    void setOnConnectionSuspendedCallback(std::function<void(Connection*)> cb);

    /* Used to set the onConnectionResumed callback function for the Connection */
    void setOnConnectionResumedCallback(std::function<void(Connection*)> cb);

    /* Used to set the onMsgReceived callback function for the Connection */
    void setOnMsgReceivedCallback(std::function<void(Connection*, pbuf::NetworkMessage)> cb);

 private:

#if !COMPILING_ON_WINDOWS
    /* Private constructor to be used by the Listener when it receives an incoming connection */
    Connection(int sockfd);

    /* Local member to store address of the peer endpoint */
    struct sockaddr_in peerAddr;
#endif

    /* The states this connection may be in */
    enum class State {
        DISCONNECTED,   /* No socket file descriptor - socket completely closed */
        SUSPENDED,      /* No socket error (yet) but inactivity / disruption detected */
        ACTIVE          /* Socket open, no disruption to connection checks */
    };

    /* Holds the current state of this connection */
    State currentState;

    /* The callback functions registered for this Connection instance */
    ConnectionCallbacks cbs;

    /* Holds the socket handle - Platform specific */
#if COMPILING_ON_WINDOWS
    SOCKET sockfd;
#else
    int sockfd;
#endif

    /* Used as a stopwatch, to count up when timing some aspect of a socket */
    double timer;

    /* Set when a ping is sent to mark that PING is sent until a message is received */
    bool pingSent;

    /* Flag to mark when pong should be sent. Not sent immediately due to object deletion worry */
    bool shouldPong;

    /* The timeout period when opening a connection. If it takes longer, fail */
    double openTimeout;

    /* The buffer into which received data will be read before decoding it */
    char recvBuffer[RECV_BUFFER_SIZE];
    int recvBufferPos;

    /* This var tracks the size of the currently-being-read NetworkMessage with recv */
    uint16_t recvMsgSize;

    /* The network message struct to parse into when receiving data */
    pbuf::NetworkMessage recvMsg;

#if !COMPILING_ON_WINDOWS
    /* We need listener as a friend to create Connections from socket descriptors */
    friend class Listener;
#endif

};

/*
 * Class representing a TCP listener socket. It listens for incoming connections and
 * notifies when a connection is accepted. The accepted "Connection" is passed as part of
 * the notification callback so it can then be used in whatever way deemed fit by the
 * initiator of the listener. ONLY IMPLEMENTED FOR NON-WINDOWS BUILDS.
 */
#if !COMPILING_ON_WINDOWS
class Listener {
 public:

    /* 
     * Constructor - calling this initiates the listener on the given port
     * and starts accepting connections. The port is the port that should be listened on.
     * The callback is a function to be called when new connections are accepted.
     * It receives a pointer to the new Connection and becomes responsible for managing
     * and freeing the connection when its use is complete.
     */
    Listener(uint16_t port, std::function<void(Connection*)> cb);
    
    /* Destructor - tear down any memory used by the Listener and close the socket */
    ~Listener();

    /* Poll method for the listener socket. This should be called regularly */
    void poll();

 private:

    /* The file descriptor pointing to the main listening socket */
    int sockfd;

    /* Callback function that will be called when a new socket connection is received */
    std::function<void(Connection*)> callback;

};

/* Empty exception type to throw when a listener fails */
class ListenerException : public std::runtime_error {
 public:
    ListenerException(const char* message) : std::runtime_error(message) {}
};

#endif /* End linux-only listener implementation */

/* Empty exception type to throw when a connection fails */
class ConnectionException : public std::runtime_error {
 public:
    ConnectionException(const char* message) : std::runtime_error(message) {}
};

#endif