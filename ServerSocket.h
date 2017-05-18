#pragma once

#include <vector>
#include "types.h"
#include "SocketUtil.h"

namespace simplesocket {
    
}

/*struct ClientInfo {
    int sock;
    struct sockaddr_in addr;
    int len;
    int id;
};

class ServerSocket {
    bool initialized = false;
    int sock = 0; //	my socket
    struct sockaddr_in addr; //	my addr
public:
    ServerSocket();

    ~ServerSocket();

    int init();

    int openServer(int port, int limit);

    int openServerUdp(int port);

    int acceptClient(ClientInfo *client);

    int receiveFromClient(ClientInfo *client, char *data, int size);

    int receiveUdpFromClient(ClientInfo *client, char *data, int size);

    int sendToClient(ClientInfo *client, char *data, int size);

    int sendUdpToClient(ClientInfo *client, char *data, int size);

    void setSockNonBlock();

    void setSockBlock();

    void setSockNonBlock(ClientInfo *client);

    void closeClientSocket(ClientInfo *client);

    void closeSocket();

    struct sockaddr_in getAddr() {
        return addr;
    }
};*/
