//
// Created by acat on 19-3-28.
//

#ifndef TRASHY_SERVER_SOCKET_H
#define TRASHY_SERVER_SOCKET_H

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <fcntl.h>
#include <cstdio>
#include <cassert>

namespace net {

namespace privatedomain
{

// default to use IPv4
class address
{
private:
    struct sockaddr_in m_sockaddr_4;

    void init_addr()
    {
        bzero(&m_sockaddr_4, sizeof(m_sockaddr_4));
        m_sockaddr_4.sin_family = AF_INET;
        m_sockaddr_4.sin_addr.s_addr = INADDR_ANY;
    }
public:
    address();
    void setip(const char *ip);
    void setport(int port);
    address(const char *ip, int port);
    const struct sockaddr_in &getaddr() const;
    ~address();
};

} // end of pridomain


class socket {
private:
    int m_fd;
    privatedomain::address m_addr;
public:
    socket(sa_family_t fm, int type, int protocol);
    void setip(const char *ip);
    void setport(int port);
    void bind();
    void listen();
    int getfd() const { return m_fd; }
    ~socket();
};


void setnonblocking(int fd);

} // end of net



#endif //TRASHY_SERVER_SOCKET_H
