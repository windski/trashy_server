//
// Created by windsky on 17-8-11.
//

#include "Server.h"
#include "tool_util.h"

namespace net {

    Server::Server(char *ip, int port) : connfd(0)
    {
        rewrite_tool::my_str str(ip);
        Creat_socket(AF_INET, str, 0);
    }

    Server::Server(char *ip) : connfd(0)
    {
        rewrite_tool::my_str str(ip);
        Creat_socket(AF_INET, str, 0);
    }

    Server::Server()
            : Creat_socket(),
              connfd(0)
    {  }

    Server::~Server() {  }

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
        struct epoll_event ev, events[MAX_EVENT];
        int nfds, epollfd;

        if((epollfd = epoll_create1(0)) == -1) {
            perror("epoll_create1 error");
            exit(EXIT_FAILURE);
        }

        ev.events = EPOLLIN;
        ev.data.fd = sockfd;
        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
            perror("epoll_ctl error");
            exit(EXIT_FAILURE);
        }

        while(true) {
            if((nfds = epoll_wait(epollfd, events, MAX_EVENT, -1)) == -1) {
                perror("epoll_wait error");
                exit(EXIT_FAILURE);
            }

            for(int n = 0; n < nfds; ++n) {
                if(events[n].data.fd == sockfd) {
                    socklen_t socklen = sizeof(sock_in.in_addr);
                    connfd = accept(sockfd, (addr_t *)&(sock_in.in_addr), &socklen);

                    if(connfd == -1) {
                        perror("accept error");
                        exit(EXIT_FAILURE);
                    }

                    setnonblocking(connfd);

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = connfd;

                    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) == -1) {
                        perror("epoll_ctl: connfd");
                        exit(EXIT_FAILURE);
                    }

                } else {
                    do_somthing(events[n].data.fd);
                    events[n].data.fd = -1;
                }
            }

        }
        return 0;
    }

    void Server::setnonblocking(int fileno) {
        int flags;

        if((flags = fcntl(fileno, F_GETFL)) < 0) {
            perror("fcntl get error");
            exit(EXIT_FAILURE);
        }

        flags |=  O_NONBLOCK;

        if(fcntl(fileno, F_SETFL, flags) < 0) {
            perror("fcntl set error");
            exit(EXIT_FAILURE);
        }
    }

    void Server::do_somthing(int fileno)
    {

        char buff[MAXLINE];
        char head[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nServer: MyServer/2.3.3 (DebianOS)\r\n\r\n";
        char send[] = "<html><title>test</title><body><h1>This is a Demo Server!!</h1></body></html>";
        ssize_t n;
        while((n = read(fileno, buff, MAXLINE)) < 0) {
            if(errno == EINTR) {
                continue;
            } else {
                perror("read error");
                exit(EXIT_FAILURE);
            }
        }
        rewrite_tool::http_parse parse(buff);
        buff[n + 1] = '\0';
        write(fileno, head, sizeof(head));
        write(fileno, send, sizeof(send));
        printf("%s", buff);

        close(fileno);
    }


}