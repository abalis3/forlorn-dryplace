#include <unistd.h>
#include <iostream>
#include <chrono>

#include "ServerLog.h"
#include "SessionList.h"
#include "Lobby.h"

#define PORT 44444
#define MIN_LOOP_PERIOD_USEC 50000 /* minimum microsec per loop (sleep if faster) */
#define USEC_PER_SEC 1000000

SessionList *sessions;
std::list<Lobby*> lobbies;

static bool sessionCanUseName(Session *sess, const std::string &name) {
    Session *sess_by_name = sessions->findByName(name);
    return ((sess_by_name == nullptr) || (sess_by_name == sess));
}

static bool addSessionToLobby(Session *sess, const std::string &lobbyName) {
    Lobby *lobby;
    if (sess->getName() == lobbyName) { /* XXX does this comparison work b/t string and string* */
        lobby = new Lobby();
        lobbies.push_back(lobby);
    } else {
        std::list<Lobby*>::const_iterator iter;
        for (iter = lobbies.begin(); iter != lobbies.end(); iter++) {
            if (iter->getHostName() == lobbyName) {
                lobby = *iter;
                break;
            }
        }
    }
    return false;
}

static void onConnAccept(Connection *conn)
{
    sessions->generateSession(conn, sessionCanUseName, addSessionToLobby);
    Logger::log() << "Connection received from " << conn->getPeerIp() << ":" << conn->getPeerPort();
}

int main()
{
    Listener *listener;
    bool running = true;

    /* Start listenening for incoming connections to the server */
    try {
        listener = new Listener(PORT, onConnAccept);
    } catch (ListenerException &exp) {
        std::cout << "Failed to create listener on port " << PORT << std::endl;
        return 1;
    }
    std::cout << "Listening on port " << PORT << std::endl;
    
    sessions = new SessionList();

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running) {

        /* Update the elapsed time for this loop for polling */
        auto elapsedTime = std::chrono::high_resolution_clock::now() - lastTime;
        double elapsedSecs = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime).count() / 1000000000.0d;
        lastTime += std::chrono::nanoseconds((uint64_t) (1000000000 * elapsedSecs));

        if ((elapsedSecs * USEC_PER_SEC) < MIN_LOOP_PERIOD_USEC) {
            usleep(MIN_LOOP_PERIOD_USEC - (elapsedSecs * USEC_PER_SEC));
        }

        Logger::update(elapsedSecs);
        listener->poll();
        sessions->pollSessions(elapsedSecs);
    }

    std::cout << "Stopping listener and destroying server" << std::endl;
    delete sessions;
    delete listener;
    return 0; 
}
