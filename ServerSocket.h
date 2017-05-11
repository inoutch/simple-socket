#pragma once
#ifdef WIN32
#include <WinSock2.h>
#else

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#endif

#include <vector>

struct ConnectionConfig {
    char name[64];
    int index;
};

struct ClientInfo {
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
};
