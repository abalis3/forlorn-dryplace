#include "Connection.h"
#include <unistd.h>
#include <iostream>

#define PORT 44444 

using namespace std;

int conns = 0;

static void onConnAccept(Connection *conn)
{
    std::cout << "Connection Received" << std::endl;
    conns++;
    delete conn;
}

int main()
{
    Listener *l = new Listener(PORT, onConnAccept);

    while (conns < 5) {
        l->poll();
    }

    delete l;
    return 0; 
}