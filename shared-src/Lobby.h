#ifndef FD__LOBBY_H
#define FD__LOBBY_H

#include <string>

extern const int LOBBY_MAX_PLAYERS; /* = 5 */

class LobbyMember {
 public:
    const std::string &getMemberName();
 private:
    std::string name;
};

class Lobby {
 public:
    /* Public constructor, initilializes state */
    Lobby();

    const std::string &getHostName();

 private:
    
    /* List of members in this lobby (some may be nullptr). Slot 0 is the host. */
    LobbyMember *members[LOBBY_MAX_PLAYERS];
};

#endif