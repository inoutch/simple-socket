#include "SocketUtil.h"

using namespace simplesocket;

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