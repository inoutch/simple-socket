#include "ServerSocket.h"
#include <errno.h>
#include <signal.h>

ServerSocket::ServerSocket() {}

ServerSocket::~ServerSocket() {}

#ifdef WIN32

int ServerSocket::init() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR) {
        return -1;
    }
    initialized = true;

    return 0;
}

int ServerSocket::acceptClient(ClientInfo* client) {
    static int id;
    if (!initialized)
        return -1;

    ClientInfo cbuf;
    cbuf.len = sizeof(cbuf.addr);
    cbuf.id = id++;
    cbuf.sock = accept(sock, (struct sockaddr*)&cbuf.addr, &cbuf.len);
    if (cbuf.sock == (unsigned int)-1)
        return -1;
    if (client)
        *client = cbuf;
    return 0;
}

void ServerSocket::setSockNonBlock() {
    unsigned long nonblocking = 1;
    ioctlsocket(sock, FIONBIO, &nonblocking);
}

void ServerSocket::setSockNonBlock(ClientInfo* client) {
    unsigned long nonblocking = 1;
    ioctlsocket(client->sock, FIONBIO, &nonblocking);
}

void ServerSocket::setSockBlock() {
    unsigned long nonblocking = 0;
    ioctlsocket(sock, FIONBIO, &nonblocking);
}

void ServerSocket::closeClientSocket(ClientInfo* client) {
    closesocket(client->sock);
}

void ServerSocket::closeSocket() { closesocket(sock); }

#else

int ServerSocket::init() {
    initialized = true;
#ifdef LINUX
    signal(SIGPIPE, SIG_IGN);
#endif
    return 0;
}

int ServerSocket::acceptClient(ClientInfo *client) {
    if (!initialized) {
        return -1;
    }

    ClientInfo clientInfo;
    clientInfo.len = sizeof(clientInfo.addr);

    clientInfo.sock = accept(sock, (struct sockaddr *) &clientInfo.addr, (socklen_t *) &clientInfo.len);
    if (clientInfo.sock <= 0) {
        return -1;
    }
    if (client) {
        *client = clientInfo;
    }
    return 0;
}

void ServerSocket::setSockNonBlock() { fcntl(sock, F_SETFL, O_NONBLOCK); }

void ServerSocket::setSockNonBlock(ClientInfo *client) {
    fcntl(client->sock, F_SETFL, O_NONBLOCK);
}

void ServerSocket::setSockBlock() {
    int val;
    val = fcntl(sock, F_GETFL, 0);
    val = val & ~(val & O_NONBLOCK);
    fcntl(sock, F_SETFL, val);
}

void ServerSocket::closeClientSocket(ClientInfo *client) {
    if (client->sock) {
        close(client->sock);
    }
    client->sock = 0;
}

void ServerSocket::closeSocket() {
    if (sock) {
        close(sock);
    }
    sock = 0;
}

#endif

int ServerSocket::openServer(int port, int limit) {
    if (!initialized)
        return -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t) port);
#ifdef WIN32
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes));
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        this->closeSocket();
        return -1;
    }

    if (listen(sock, limit) != 0) {
        perror("listen");
        this->closeSocket();
        return -1;
    }
#if !defined(WIN32) && !defined(LINUX) && !defined(ANDROID)
    setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (char*)&yes, sizeof(yes));
#endif
    return 0;
}

int ServerSocket::openServerUdp(int port) {
    if (!initialized) {
        return -1;
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t) port);
#ifdef WIN32
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    addr.sin_addr.s_addr = INADDR_ANY;
#endif
    char yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    return 0;
}

int ServerSocket::receiveFromClient(ClientInfo *client, char *data, int size) {
    int re = (int) recv(client->sock, data, (size_t) size, 0);
    if (re == -1) {
        switch (errno) {
            case EPIPE:
                return -2;
            default:
                return -1;
        }
    } else {
        return re;
    }
}

int ServerSocket::receiveUdpFromClient(ClientInfo *client, char *data, int size) {
    client->len = sizeof(client->addr);
#if defined(WIN32) || defined(ANDROID)
    int re = (int)recvfrom(sock, data, size, 0, (struct sockaddr*)&client->addr, (int*)&client->len);
#else
    int re = (int) recvfrom(sock, data, (size_t) size, 0, (struct sockaddr *) &client->addr,
                            (unsigned int *) &client->len);
#endif
    if (re == -1) {
        switch (errno) {
            case EPIPE:
                return -2;
            default:
                return -1;
        }
    } else {
        return re;
    }
}

int ServerSocket::sendToClient(ClientInfo *client, char *data, int size) {
    int re = (int) send(client->sock, data, (size_t) size, 0);
    if (re == -1) {
        switch (errno) {
            case EPIPE:
                return -2;
            default:
                return -1;
        }
    } else {
        return re;
    }
}

int ServerSocket::sendUdpToClient(ClientInfo *client, char *data, int size) {
    int re = (int) sendto(sock, data, (size_t) size, 0, (const struct sockaddr *) &client->addr, sizeof(client->addr));
    if (re == -1) {
        switch (errno) {
            case EPIPE:
                return -2;
            default:
                return -1;
        }
    } else {
        return re;
    }
}
