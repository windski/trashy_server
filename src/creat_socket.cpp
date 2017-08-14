//
// Created by windsky on 17-8-10.
//

#include "creat_socket.h"

namespace net {

    Creat_socket::Creat_socket()
    {
        init_();
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }

    Creat_socket::Creat_socket(sa_family_t family, rewrite_tool::my_str & ip, int opt)
    {
        if(family == AF_INET){
            init_(AF_INET, ip.to_addr(IPV4), opt);
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
        } else if(family == AF_INET6){
            init_(AF_INET6, ip.to_addr(IPV6), opt);
            sockfd = socket(AF_INET6, SOCK_STREAM, 0);
        }

    }

    void Creat_socket::init_(sa_family_t family, in_addr_t ip, int opt)
    {
        memset(&sock_in.in_addr, 0, sizeof(sock_in.in_addr));
        sock_in.in_addr.sin_port = htons(DEBUG_PORT);

        if(ip != INADDR_ANY) {
            sock_in.in_addr.sin_addr.s_addr = ip;
        }

        if(family == AF_INET) {
            sock_in.in_addr.sin_family = AF_INET;
            sock_in.ipver = IPV4;
        } else if(family == AF_INET6) {
            sock_in.in_addr.sin_family = AF_INET6;
            sock_in.ipver = IPV6;
        }

    }

    const addr_in_t Creat_socket::get_addr(void) const
    {
        return sock_in.in_addr;
    }

    socklen_t Creat_socket::get_addr_len() const
    {
        return sizeof(sock_in.in_addr);
    }

    const int Creat_socket::get_sock_fileno() const
    {
        return sockfd;
    }

    Creat_socket::~Creat_socket() = default;

} // end of net
