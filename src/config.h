//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_CONFIG_H
#define TRASHY_SERVER_CONFIG_H

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 80
#endif

#ifndef DEBUG_PORT
#define DEBUG_PORT 23333
#endif


#include <sys/socket.h>
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cerrno>
#include <sys/epoll.h>
#include <fcntl.h>

#include <cstdarg>

#include <dirent.h>
#include <sys/select.h>

#include "log.h"

namespace net {
    const size_t MAXLINE = 102400;
    const size_t MAXQUE = 1024;
    const size_t MAX_EVENT = 50;
    const size_t MAX_URI_LEN = 200;

    enum IP_VERSION { IPV4, IPV6 };

    typedef struct sockaddr_in addr_in_t;
    typedef struct sockaddr addr_t;

    struct sock_s {
        addr_in_t in_addr;
        IP_VERSION ipver;
    };

    typedef struct sock_s sock_t;

} // end of net

#ifndef SERVER_NAME
#define SERVER_NAME "Server: My server/2.3.3 (Debain OS)"
#endif

#ifndef HTTP_VER_STR
#define HTTP_VER_STR "HTTP/1.1"
#endif


#endif //TRASHY_SERVER_CONFIG_H
