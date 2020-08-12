#include "Connection.h"

#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

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

Connection::Connection(std::string ip, uint16_t port)
{
    currentState = State::DISCONNECTED;
}

Connection::~Connection()
{
    if (sockfd != 0) {
        close(sockfd);
    }
}

Connection::Connection(int sockfd)
{
    currentState = State::ACTIVE;
    this->sockfd = sockfd;
}

/* Implementation for Listener class */
#if not(COMPILING_ON_WINDOWS)

Listener::Listener(uint16_t port, std::function<void(Connection*)> cb)
{
    int flags;
    int err;

    callback = cb;

    /* Create the socket file descriptor */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) { 
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
        callback(new Connection(newsock));
        newsock = accept(sockfd, NULL, NULL);
    }

    /* We got -1 as error. Check if it's just nonblocking notification or something worse */
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
        throw ListenerException("Socket accept failed");
    }
}

#endif