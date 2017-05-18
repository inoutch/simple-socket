#ifndef EXAMPLES_SOCKETUTIL_H
#define EXAMPLES_SOCKETUTIL_H

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
#include <signal.h>

#endif

namespace simplesocket {
    class SocketUtil {
    public:
        static void setNoSigPipe(int sock);
        static void setNonBlock(int sock);
        static void setReuseAddr(int sock);
    };
}


#endif //EXAMPLES_SOCKETUTIL_H
