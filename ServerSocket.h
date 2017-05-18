#pragma once

#include <vector>
#include "types.h"
#include "SocketUtil.h"

namespace simplesocket {
    typedef struct {
        int sock;
        struct sockaddr_in addr;
        socklen_t len;
    } ClientInfo;

    class ServerSocket {
    private:
        int sock = 0;
        struct sockaddr_in serverAddr;
        IPType type = None;

    public:
        ServerSocket();

        ~ServerSocket();

        int openServer(uint16_t port, IPType type, int limit = 1);

        int acceptClient(ClientInfo *newClientInfo);

        ssize_t sendToClient(const ClientInfo *clientInfo, const char *data, size_t size);

        ssize_t receiveFromClient(const ClientInfo *clientInfo, char *data, size_t size);

        ssize_t receiveFromSomeone(ClientInfo *someone, char *data, size_t size);

        void closeClient(const ClientInfo *clientInfo);

        void closeSocket();
    };
}