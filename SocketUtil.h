#ifndef EXAMPLES_SOCKETUTIL_H
#define EXAMPLES_SOCKETUTIL_H

#ifdef WIN32
#include <WinSock2.h>
#else

#include <string>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#endif

#ifdef LINUX
#include <signal.h>
#endif

namespace simplesocket {
    class SocketUtil {
    public:
        static struct sockaddr_in getAddr(in_addr_t addr, uint16_t port);

        static ssize_t checkError(ssize_t result);

        static void setNoSigPipe(int sock);

        static void setNonBlock(int sock);

        static void setReuseAddr(int sock);
    };
}


#endif //EXAMPLES_SOCKETUTIL_H
