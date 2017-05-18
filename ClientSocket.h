#pragma once

#include <vector>
#include "types.h"
#include "SocketUtil.h"

namespace simplesocket {
    typedef struct {
        int sock;
        struct sockaddr_in addr;
        socklen_t len;
    } ServerInfo;

    class ClientSocket {
    private:
        int sock = -1;
        struct sockaddr_in serverAddr;
        struct sockaddr_in clientAddr;
        IPType type = None;

        ssize_t sendToServerTcp(const char *data, size_t size);

        ssize_t sendToServerUdp(const char *data, size_t size);

    public:
        ClientSocket();

        ~ClientSocket();

        int connectServer(const char *addr, uint16_t port, IPType type, bool nonblock = false);

        ssize_t sendToServer(const char *data, size_t size);

        ssize_t sendToServer(const ServerInfo *server, const char *data, size_t size);

        ssize_t sendByBroadcast(const char *data, size_t size);

        ssize_t receiveFromServer(char *data, size_t size);

        ssize_t receiveFromSomeone(ServerInfo *someone, char *data, size_t size);

        void closeSocket();

        ServerInfo getEmptyServerInfo();
    };
}