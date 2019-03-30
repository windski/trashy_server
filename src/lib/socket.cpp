//
// Created by acat on 19-3-28.
//

#include "socket.h"
#include <fcntl.h>
#include <cstdio>

namespace net {

namespace privatedomain
{
address::address()
{
    init_addr();
}

address::address(const char *ip, int port)
{
    init_addr();

    if (ip != nullptr) {
        m_sockaddr_4.sin_addr.s_addr = ::inet_addr(ip);
    }

    m_sockaddr_4.sin_port = ::htons(port);
}

void address::setip(const char *ip)
{
    m_sockaddr_4.sin_addr.s_addr = ::inet_addr(ip);
}

void address::setport(const int port)
{
    m_sockaddr_4.sin_port = ::htons(port);
}

const struct sockaddr_in &
    address::getaddr() const
{
    return m_sockaddr_4;
}

address::~address()
{
}


} // end of privatedomain



socket::socket(sa_family_t fm, int type, int protocol)
: m_addr(), m_fd(::socket(fm, type, protocol))
{
}

void socket::setip(const char *ip)
{
    m_addr.setip(ip);
}

void socket::setport(int port)
{
    m_addr.setport(port);
}

int socket::bind()
{
    return ::bind(m_fd, (struct sockaddr *)(&m_addr.getaddr()), sizeof(struct sockaddr));
}

int socket::listen()
{
    return ::listen(m_fd, 1024);
}

socket::~socket()
{
    ::shutdown(m_fd, SHUT_RDWR);
    close(m_fd);
}

void setnonblocking(int fd)
{
    assert(fd > 0);
    int flag;
    if((flag = fcntl(fd, F_GETFL)) < 0) {
        perror("get m_fd failure");
        return ;
    }

    flag |= O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flag) < 0) {
        perror("set m_fd failure");
        return ;
    }
}


}
