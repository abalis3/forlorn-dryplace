#ifndef FD__SERVERSESSION_H
#define FD__SERVERSESSION_H

#include "Connection.h"

/*
 * This class defines an abstraction layer for the game client to communicate with
 * the server and be notified of events occurring on the server, or of connection status
 * changes. It allows the consumer to perform a predefined set of actions (ultimately resulting
 * in messages transmitted to/from the server) and to register a callback to be informed
 * of events arising from the connection/server. It is a stateful "session" that retains
 * info about the connection to the server and established parameters for the interaction.
 */
class ServerSession {
 public:

    /*
     * Default constructor - create an instance. Note - connection won't be opened
     * until the open() function is called in this class
     */
    ServerSession();

    /* Destructor - used to clean up objects and memory used by this instance */
    ~ServerSession();

    enum class Event {
        NONE,
        CONNECTION_FAILED,    /* Connection not made or issue before name accept/reject */
        CONNECTION_SUSPENDED, /* AFTER name accepted/rejected, connection suspended */
        CONNECTION_RESUMED,   /* A suspended connection was restored to healthy state */
        CONNECTION_LOST,      /* AFTER name accepted/rejected, loss of connection */
        NAME_ACCEPTED,
        NAME_REJECTED,
    };

    /* Register a callback function to be notified of events for this ServerSession */
    void registerCallback(std::function<void(Event)> cb);

    /* Attempt to open a server session using the given name */
    void open(std::string name);

    /* End any existing session with the server (releasing name) and close any connection */
    void close();

    /* Poll events for this ServerSession. Should be called regularly */
    void poll(double secs);

    /* Get the remaining time until a suspended session is disconnected */
    int getSuspendedTimeLeft();

 private:

    /* This is the connection instance used to communicate with the server */
    Connection *connection;

    /* Holds the callback function that will be called on notable events */
    std::function<void(Event)> callback;

    /* Holds the name registered (or being registered) for this ServerSession */
    std::string name;

    /* Set to true if the name request fails so we can keep retrying */
    bool retryNameRequest;

    /* Set to true if currently in process of connecting / requesting name */
    bool requestingName;

    /* Sends a name request protobuf message over the connection containing the name class member */
    void sendNameRequest();

    /* Transmits a network message protobuf over the wire to the server */
    void sendNetworkMessage(pbuf::NetworkMessage &msg);

    /* 
     * Callback functions that will be registered with the connection to be notified
     * of connection status updates
     */
    void onConnectSuccess(Connection *conn);
    void onConnectFail(Connection *conn);
    void onConnectionLost(Connection *conn);
    void onConnectionSuspended(Connection *conn);
    void onConnectionResumed(Connection *conn);
    void onMsgReceived(Connection *conn, pbuf::NetworkMessage msg);

};

#endif