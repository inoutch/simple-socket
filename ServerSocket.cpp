#include "ServerSocket.h"

using namespace simplesocket;

ServerSocket::ServerSocket() {
#ifdef LINUX
    //  ignore sigpipe
    signal(SIGPIPE, SIG_IGN);
#endif
    sock = 0;
}

ServerSocket::~ServerSocket() {
    closeSocket();
}

int ServerSocket::openServer(uint16_t port, IPType type, int limit) {
#ifdef WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
        return -1;
    }
#endif
    closeSocket();

    this->type = type;

    if (type == TCP) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
    } else if (type == UDP) {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
    } else {
        return -1;
    }
    serverAddr = SocketUtil::getAddr(INADDR_ANY, port);

    SocketUtil::setReuseAddr(sock);

    if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        closeSocket();
        return -1;
    }

    if (type == IPType::TCP) {
        if (listen(sock, limit) != 0) {
            perror("listen");
            closeSocket();
            return -1;
        }
    }

    SocketUtil::setNoSigPipe(sock);
    return 0;
}

int ServerSocket::acceptClient(ClientInfo *newClientInfo) {
    if (sock <= 0) {
        return -1;
    }

    ClientInfo clientInfo = {};
    clientInfo.len = sizeof(clientInfo.addr);

    clientInfo.sock = accept(sock, (struct sockaddr *) &clientInfo.addr, (socklen_t *) &clientInfo.len);
    if (clientInfo.sock <= 0) {
        return -1;
    }
    if (newClientInfo) {
        *newClientInfo = clientInfo;
    }
    return 0;
}

ssize_t ServerSocket::receiveFromClient(const ClientInfo *clientInfo, char *data, size_t size) {
    return SocketUtil::checkError(recv(clientInfo->sock, data, size, 0));
}

ssize_t ServerSocket::receiveFromSomeone(ClientInfo *someone, char *data, size_t size) {
#if defined(WIN32) || defined(ANDROID)
    return SocketUtil::checkError(
            recvfrom(sock, data, serverAddr.sin_len, 0, (struct sockaddr*)&client->addr, (int*)&client->len));
#else
    return SocketUtil::checkError(
            recvfrom(sock, data, size, 0, (struct sockaddr *) &someone->addr, &someone->len));
#endif

}

void ServerSocket::closeClient(const ClientInfo *clientInfo) {
    if (clientInfo->sock > 0) {
        close(clientInfo->sock);
    }
}

void ServerSocket::closeSocket() {
    if (sock > 0) {
        close(sock);
    }
    type = None;
}

ssize_t ServerSocket::sendToClient(const ClientInfo *clientInfo, const char *data, size_t size) {
    if (type == UDP) {
        return SocketUtil::checkError(
                sendto(sock, data, size, 0, (const struct sockaddr *) &clientInfo->addr, clientInfo->len));
    } else if (type == TCP) {
        return SocketUtil::checkError(send(clientInfo->sock, data, size, 0));
    }
    return 0;
}