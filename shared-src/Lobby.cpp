#include "Lobby.h"

const int LOBBY_MAX_PLAYERS = 5;
static const std::string EMPTY_STR = "";

const std::string &LobbyMember::getMemberName() {
    return name;
}

const std::string &Lobby::getHostName() {
    if (members[0] == nullptr) {
        return EMPTY_STR;
    }

    return members[0]->getMemberName();
}

Lobby::Lobby() {
    int i;

    for (i = 0; i < LOBBY_MAX_PLAYERS; i++) {
        members[i] = nullptr;
    }
}