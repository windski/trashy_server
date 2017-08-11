//
// Created by windsky on 17-8-11.
//

#include "Server.h"

namespace net {

    Server::Server(char *ip, int port)
    {
        rewrite_tool::my_str str(ip);
        Creat_socket(AF_INET, str, 0);
    }

    Server::Server(char *ip)
    {
        rewrite_tool::my_str str(ip);
        Creat_socket(AF_INET, str, 0);
    }

    Server::Server() : Creat_socket() { }

    Server::~Server() { }

    int Server::config()
    {
        int status = 0x00;    // status描述状态, 共两位.   0x01 bind 出错, 0x10 listen 出错,  0x11 都有问题, 0x00 .. 正常.
        if(bind(sockfd, (addr_t *)&(sock_in.in_addr), sizeof(sock_in.in_addr)) != 0)
            status = 0x01;
        if(listen(sockfd, MAXQUE) != 0) {
            if(status == 0x00) {
                status = 0x10;
            } else {
                status = 0x11;
            }
        }
        return (status == 0x00) ? 0 : status;
    }

    int Server::run()
    {


    }


}