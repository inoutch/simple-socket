#include "ClientSocket.h"
#include <errno.h>

#ifdef WIN32
#include <ws2tcpip.h>
#endif

using namespace simplesocket;

ClientSocket::ClientSocket() {
#ifdef LINUX
    //  ignore sigpipe
    signal(SIGPIPE, SIG_IGN);
#endif
    sock = 0;
}

ClientSocket::~ClientSocket() {
    closeSocket();
}

int ClientSocket::connectServer(const char *addr, uint16_t port, IPType type, bool nonblock) {
#ifdef WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
        return -1;
    }
#endif

    closeSocket();
    this->type = type;

    if (type == IPType::TCP) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return -1;
        }
    } else if (type == IPType::UDP) {
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            return -1;
        }
    }
    SocketUtil::setNoSigPipe(sock);

    if (nonblock) {
        SocketUtil::setNonBlock(sock);
    }

    clientAddr = SocketUtil::getAddr(inet_addr(addr), port);
    if (type == IPType::TCP) {
        if (connect(sock, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0) {
            closeSocket();
            return -1;
        }
    } else if (type == IPType::UDP) {
        serverAddr = SocketUtil::getAddr(INADDR_ANY, port);
        SocketUtil::setReuseAddr(sock);
        if (bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
            perror("bind");
            return -1;
        }
    }

    return 0;
}

void ClientSocket::closeSocket() {
    if (sock < 0) {
        return;
    }
#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    sock = -1;
    type = None;
}

ServerInfo ClientSocket::getEmptyServerInfo() {
    ServerInfo serverInfo = {};
    serverInfo.len = sizeof(serverInfo.addr);
    return serverInfo;
}

ssize_t ClientSocket::sendToServer(const char *data, size_t size) {
    if (type == TCP) {
        return sendToServerTcp(data, size);
    } else if (type == UDP) {
        return sendToServerUdp(data, size);
    }
}

ssize_t ClientSocket::sendByBroadcast(const char *data, size_t size) {
    if (type != UDP) {
        return 0;
    }
    int opt = 1, tmp = 0;
    socklen_t len = sizeof(opt);
    getsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &tmp, &len);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &opt, len);

    auto ret = SocketUtil::checkError(sendToServerUdp(data, size));

    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *) &tmp, len);
    return ret;
}

ssize_t ClientSocket::sendToServerTcp(const char *data, size_t size) {
    return SocketUtil::checkError(send(sock, data, size, 0));
}

ssize_t ClientSocket::sendToServerUdp(const char *data, size_t size) {
    return SocketUtil::checkError(
            sendto(sock, data, size, 0, (const struct sockaddr *) &clientAddr, sizeof(clientAddr)));
}

ssize_t ClientSocket::sendToServer(const ServerInfo *server, const char *data, size_t size) {
    if (type != UDP) {
        return 0;
    }
    return SocketUtil::checkError(
            sendto(sock, data, size, 0, (const struct sockaddr *) &server->addr, sizeof(server->addr)));
}

ssize_t ClientSocket::receiveFromServer(char *data, size_t size) {
    return SocketUtil::checkError(recv(sock, data, size, 0));
}

ssize_t ClientSocket::receiveFromSomeone(ServerInfo *server, char *data, size_t size) {
#if defined(WIN32) || defined(ANDROID)
    return SocketUtil::checkError(recvfrom(sock, data, size, 0, (struct sockaddr *) &server->addr, (int*)&server->len));
#else
    return SocketUtil::checkError(recvfrom(sock, data, size, 0, (struct sockaddr *) &server->addr, &server->len));
#endif
}
