#include "Connection.h"

#if COMPILING_ON_WINDOWS

#include <Ws2tcpip.h>

#else /* Compiling on linux/mac */

#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#endif

#include <iostream>

/* How many connections can be accepted at TCP level before handling with accept() */
static const int SOCKET_ACCEPT_BACKLOG = 10;

/* Implementation for Connection class */

void Connection::init()
{
#if COMPILING_ON_WINDOWS
WSADATA wsa_data;
WSAStartup(MAKEWORD(1,1), &wsa_data);
#endif
}

void Connection::quit()
{
#if COMPILING_ON_WINDOWS
WSACleanup();
#endif
}

Connection::Connection(std::string ip, uint16_t port, double timeout, ConnectionCallbacks &callbacks)
{
    int flags;
    int err;

    struct sockaddr_in address;
    address.sin_family = AF_INET,
    address.sin_port = htons(port),
    
    /* Parse the ip addr from the given ip string */
    err = inet_pton(AF_INET, ip.c_str(), &address.sin_addr);
    if (err <= 0) {
        throw ConnectionException("Unable to parse IP address for socket");
    }

    /* Create the socket file descriptor */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
#if COMPILING_ON_WINDOWS
    if (sockfd == INVALID_SOCKET) {
#else
    if (sockfd < 0) {
#endif
        throw ConnectionException("Socket creation failed");
    }

    /* Set the socket to nonblocking mode */
#if COMPILING_ON_WINDOWS
    u_long nbmode = 1;
    err = ioctlsocket(sockfd, FIONBIO, &nbmode);
    if (err == SOCKET_ERROR) {
#else
    flags = fcntl(sockfd, F_GETFL, 0);
    err = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if (err == -1) {
#endif
        throw ConnectionException("Failed to set socket to nonblocking mode");
    }

    err = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
    if (err != 0) {
#if COMPILING_ON_WINDOWS
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
        if (errno != EAGAIN && errno != EINPROGRESS) {
#endif
            /* Immediate failure */
            throw ConnectionException("Failed to connect to socket endpoint");
        }
    }

    /* 
     * Asynchronous connect attempt in progress (I expect this is most common)
     * - OR -
     * Immediate success - can this even happen...?
     * 
     * Both will end up resolving through poll() testing socket connect status
     */
    currentState = State::DISCONNECTED;
    openTimeout = timeout;
    cbs = callbacks;
    timer = 0;
    recvBufferPos = 0;
}

Connection::~Connection()
{
#if COMPILING_ON_WINDOWS
    if (sockfd != INVALID_SOCKET) {
        closesocket(sockfd);
    }
#else
    if (sockfd >= 0) {
        close(sockfd);
    }
#endif
}

#if !COMPILING_ON_WINDOWS
Connection::Connection(int sockfd)
{
    currentState = State::ACTIVE;
    this->sockfd = sockfd;
    recvBufferPos = 0;
    cbs.onConnectFail = nullptr;
    cbs.onConnectionLost = nullptr;
    cbs.onConnectSuccess = nullptr;
    cbs.onMsgReceived = nullptr;

    /* Make a record of the remote endpoint details */
    socklen_t addr_len = sizeof(peerAddr);
    getpeername(sockfd, (struct sockaddr*) &peerAddr, &addr_len);
}

std::string Connection::getPeerIp()
{
    return inet_ntoa(peerAddr.sin_addr);
}

uint16_t Connection::getPeerPort()
{
    return ntohs(peerAddr.sin_port);
}
#endif

void Connection::sendNetworkMessage(pbuf::NetworkMessage &msg)
{
#if COMPILING_ON_WINDOWS
    int res;
#else
    ssize_t res;
#endif

    if (currentState == State::DISCONNECTED) {
        throw ConnectionException("Cannot send data over closed connection");
    }

    std::string serialized = msg.SerializeAsString();
    if (serialized.empty()) {
        throw ConnectionException("Error forming name request message");
    }

    uint16_t serializedLen = htons(serialized.length());
#if COMPILING_ON_WINDOWS
    res = send(sockfd, (char*) &serializedLen, 2, 0);
#else
    res = send(sockfd, &serializedLen, 2, 0);
#endif
    if (res != 2) {
        /* 
         * Lost conn or full buffer. In either case, fail outright for now.
         * In the future, consider handling full buffer case if it ever actually happens
         */
#if COMPILING_ON_WINDOWS
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
#else
        close(sockfd);
        sockfd = -1;
#endif
        currentState = State::DISCONNECTED;
        if (cbs.onConnectionLost != nullptr) {
            cbs.onConnectionLost(this);
        }
        return;
    }

    res = send(sockfd, serialized.c_str(), serialized.length(), 0);
    if (res != serialized.length()) {
        /* 
         * Lost conn or full buffer. In either case, fail outright for now.
         * In the future, consider handling full buffer case if it ever actually happens
         */
#if COMPILING_ON_WINDOWS
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;
#else
        close(sockfd);
        sockfd = -1;
#endif
        currentState = State::DISCONNECTED;
        if (cbs.onConnectionLost != nullptr) {
            cbs.onConnectionLost(this);
        }
        return;
    }
}

void Connection::poll(double secs)
{
#if COMPILING_ON_WINDOWS
    if (currentState == State::DISCONNECTED && sockfd != INVALID_SOCKET) {
#else
    if (currentState == State::DISCONNECTED && sockfd >= 0) {
#endif
        /* Waiting for socket connect nonblocking result */

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);

        /* Immediate return (poll) of 'select' function */
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int err = select(sockfd+1, NULL, &fdset, NULL, &tv);

#if COMPILING_ON_WINDOWS
        if (err == SOCKET_ERROR) {
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            throw ConnectionException("Error polling for connection status");
        }
        /* err = 0 means timeout for windows and falls through as we want */
#else
        if (err < 0) {
            if (errno != EINTR) { /* Fall through for 'timeout' */
                /* Error polling socket for connection status */
                close(sockfd);
                sockfd = -1;
                throw ConnectionException("Error polling for connection status");
            }
        }
#endif
        else if (err > 0) {

            int optval;
            socklen_t optlen = sizeof(int);
#if COMPILING_ON_WINDOWS
            err = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *) &optval, &optlen);
            if (err != 0 || optval != 0) {
                /* Error on socket connection */
                closesocket(sockfd);
                sockfd = INVALID_SOCKET;
#else
            err = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void *) &optval, &optlen);
            if (err < 0 || optval != 0) {
                /* Error on socket connection */
                close(sockfd);
                sockfd = -1;
#endif
                if (cbs.onConnectFail != nullptr) {
                    cbs.onConnectFail(this);
                }
                return;
            }

            /* Success */
            currentState = State::ACTIVE;
            if (cbs.onConnectSuccess != nullptr) {
                cbs.onConnectSuccess(this);
            }
            return;
        }

        /* No interesting result. Make sure we didn't time out and keep waiting */
        timer += secs;
        if (timer > openTimeout) {
            /* Timed out waiting for connection */
#if COMPILING_ON_WINDOWS
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
#else
            close(sockfd);
            sockfd = -1;
#endif
            if (cbs.onConnectFail != nullptr) {
                cbs.onConnectFail(this);
            }
            return;
        }

#if COMPILING_ON_WINDOWS
    } else if (sockfd != INVALID_SOCKET) {
#else
    } else if (sockfd >= 0) {
#endif
        /* We have an active / suspended connection. Try receiving */
        
        bool recv_again = true;

#if COMPILING_ON_WINDOWS
        int res;
#else
        ssize_t res;
#endif

        while (recv_again) {
            recv_again = false;

            /* Look for the 2-byte length prefix if we haven't received it yet */
            if (recvBufferPos < 2) {
                res = recv(sockfd, &recvBuffer[recvBufferPos], 2 - recvBufferPos, 0);
#if COMPILING_ON_WINDOWS
                if (res == 0 || (res == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {
                    /* Uh oh - a real error and not just nonblocking flagging (or graceful shutdown) */
                    closesocket(sockfd);
                    sockfd = INVALID_SOCKET;
#else
                if (res == 0 || (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                    /* Uh oh - a real error and not just nonblocking flagging (or graceful shutdown) */
                    close(sockfd);
                    sockfd = -1;
#endif
                    currentState = State::DISCONNECTED;
                    if (cbs.onConnectionLost != nullptr) {
                        cbs.onConnectionLost(this);
                    }
                    return;
                } else if (res > 0) {
                    /* We got data! */
                    recvBufferPos += res;
                    if (recvBufferPos == 2) {
                        recvMsgSize = ntohs(*((uint16_t*) &recvBuffer[0]));
                    }
                }
            }

            /* Read recvMsgSize bytes of data to actually be parsed as a NetworkMessage protobuf */
            if (recvBufferPos >= 2) {
                res = recv(sockfd, &recvBuffer[recvBufferPos], (recvMsgSize + 2) - recvBufferPos, 0);
#if COMPILING_ON_WINDOWS
                if (res == 0 || (res == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {
                    /* Uh oh - a real error and not just nonblocking flagging (or graceful shutdown) */
                    closesocket(sockfd);
                    sockfd = INVALID_SOCKET;
#else
                if (res == 0 || (res == -1 && errno != EAGAIN && errno != EWOULDBLOCK)) {
                    /* Uh oh - a real error and not just nonblocking flagging (or graceful shutdown) */
                    close(sockfd);
                    sockfd = -1;
#endif
                    currentState = State::DISCONNECTED;
                    if (cbs.onConnectionLost != nullptr) {
                        cbs.onConnectionLost(this);
                    }
                    return;
                } else if (res > 0) {
                    /* We got data! */
                    recvBufferPos += res;
                    if (recvBufferPos == 2 + recvMsgSize) {
                        /* Got the whole message! Let's parse it */
                        bool successfulParse = recvMsg.ParseFromArray(&recvBuffer[2], recvMsgSize);
                        if (!successfulParse) {
                            /* Parsing failed. No saving this connection now. */
#if COMPILING_ON_WINDOWS
                            closesocket(sockfd);
                            sockfd = INVALID_SOCKET;
#else
                            close(sockfd);
                            sockfd = -1;
#endif
                            currentState = State::DISCONNECTED;
                            if (cbs.onConnectionLost != nullptr) {
                                cbs.onConnectionLost(this);
                            }
                            return;
                        }
                        if (cbs.onMsgReceived != nullptr) {
                            cbs.onMsgReceived(this, recvMsg);
                        }
                        recvBufferPos = 0; /* Prepare to read next message */
                        recv_again = true;
                    }
                }
            }
        }
    }
}

void Connection::setOnConnectionLostCallback(std::function<void(Connection*)> cb)
{
    cbs.onConnectionLost = cb;
}

void Connection::setOnMsgReceivedCallback(std::function<void(Connection*, pbuf::NetworkMessage)> cb)
{
    cbs.onMsgReceived = cb;
}


/* Implementation for Listener class */
#if !COMPILING_ON_WINDOWS

Listener::Listener(uint16_t port, std::function<void(Connection*)> cb)
{
    int flags;
    int err;

    callback = cb;

    /* Create the socket file descriptor */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { 
        throw ListenerException("Socket creation failed");
    }

    /* Set the socket to nonblocking mode */
    flags = fcntl(sockfd, F_GETFL, 0);
    err = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if (err == -1) {
        throw ListenerException("Failed to set socket to nonblocking mode");
    }

    /* Allow address and port reuse for reliable binding */
    const int optVal = 1;
    err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optVal, sizeof(optVal));
    if (err < 0) {
        throw ListenerException("Socket options set failed");
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = (in_addr) {
            .s_addr = INADDR_ANY
        }
    };

    /* Bind to port specified in parameter */
    err = bind(sockfd, (struct sockaddr *) &address, sizeof(address));
    if (err < 0) { 
        throw ListenerException("Socket failed to bind");
    }

    /* Start listening */
    err = listen(sockfd, SOCKET_ACCEPT_BACKLOG);
    if (err < 0) { 
        throw ListenerException("Socket failed to start listening");
    }
}

Listener::~Listener()
{
    close(sockfd);
}

void Listener::poll()
{
    int newsock = accept(sockfd, NULL, NULL);
    while (newsock != -1) {
        
        /* Set the socket to nonblocking mode */
        flags = fcntl(sockfd, F_GETFL, 0);
        err = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        if (err == -1) {
            throw ListenerException("Failed to set new socket to nonblocking mode");
        }

        callback(new Connection(newsock));
        
        newsock = accept(sockfd, NULL, NULL);
    }

    /* We got -1 as error. Check if it's just nonblocking notification or something worse */
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
        throw ListenerException("Socket accept failed");
    }
}

#endif