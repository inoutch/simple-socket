#include "SocketUtil.h"

using namespace simplesocket;

ssize_t SocketUtil::checkError(ssize_t result) {
    if (result != (ssize_t) -1) {
        return result;
    }
    switch (errno) {
        case EPIPE:
            return -2;
        default:
            return -1;
    }
}

struct sockaddr_in SocketUtil::getAddr(in_addr_t addr, uint16_t port) {
    struct sockaddr_in ret;

#if  !defined(LINUX) && !defined(ANDROID)
    ret.sin_len = sizeof(ret.sin_addr);
#endif
    ret.sin_family = AF_INET;
    ret.sin_port = htons(port);
#ifdef WIN32
    inet_pton(AF_INET, addr, &addr.sin_addr);
#else
    ret.sin_addr.s_addr = addr;
#endif
    memset(ret.sin_zero, 0, sizeof(ret.sin_zero));
    return ret;
}

void SocketUtil::setReuseAddr(int sock) {
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &yes, sizeof(yes));
}

void SocketUtil::setNoSigPipe(int sock) {
#if !defined(ANDROID) && !defined(LINUX)
    int set = 1;
    setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof(int));
#endif
}

void SocketUtil::setNonBlock(int sock) {
#ifdef WIN32
    unsigned long nonblocking = 1;
    ioctlsocket(sock, FIONBIO, &nonblocking);
#endif
    fcntl(sock, F_SETFL, O_NONBLOCK);
}