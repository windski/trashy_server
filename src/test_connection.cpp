//
// Created by acat on 19-4-4.
//

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <cassert>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <csignal>

#include "socket.h"

static const char demo_data[] = "GET /home.html HTTP/1.1\n"
                                "Host: developer.mozilla.org\n"
                                "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\n"
                                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n"
                                "Accept-Language: en-US,en;q=0.5\n"
                                "Accept-Encoding: gzip, deflate, br\n"
                                "Referer: https://developer.mozilla.org/testpage.html\n"
                                "Connection: keep-alive\n"
                                "Upgrade-Insecure-Requests: 1\n"
                                "If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT\n"
                                "If-None-Match: \"c561c68d0ba92bbeb8b0fff2a9199f722e3a621a\"\n"
                                "Cache-Control: max-age=0";

bool read_once(int sockfd, char *buff, int size)
{
    int bytes = 0;
    memset(buff, 0, size);
    bytes = recv(sockfd, buff, size, 0);
    if(bytes == -1) {
        return false;
    } else if(bytes == 0) {
        return false;
    }
    printf("read %d bytes data form sock", size);

    return true;
}


bool write_bytes(int sockfd, char *buff, int size)
{
    int bytes = 0;
    printf("send %d bytes to ...", size);
    while(1) {
        bytes = send(sockfd, buff, size, 0);
        if(bytes == -1) {
            return false;
        } else if(bytes == 0) {
            return false;
        }

        size -= bytes;
        buff = buff + bytes;
        if(size <= 0) {
            return true;
        }
    }
}


void addfd(int epollfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLOUT | EPOLLIN | EPOLLERR;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    net::setnonblocking(fd);
}


void start_conn(int epollfd, int nums, const char *ip, int port)
{
    struct sockaddr_in sockaddr;
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_port = htons(port);
    sockaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sockaddr.sin_addr);

    for(int i = 0; i < nums; i++) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        printf("create 1 sock\n");
        if(sockfd < 0) {
            perror("socket");
            continue;
        }

        if(connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == 0) {
            printf("create a connection\n");
            addfd(epollfd, sockfd);
        }
    }
}


void close_conn(int epollfd, int sockfd)
{
    ::epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, 0);
    close(sockfd);
}



int main(int argc, const char *args[])
{
    if(argc < 3) {
        ::fprintf(stderr, "usage: %s address port", args[0]);
        return -1;
    }

    int epollfd = epoll_create(100);
    start_conn(epollfd, 10000, args[1], atoi(args[2]));

    struct epoll_event events[1000];
    char buff[1024];

    while(true) {
        int fds = epoll_wait(epollfd, events, 1000, 2000);

        for(int i = 0; i < fds; i++) {
            int connfd = events[i].data.fd;
            if(events[i].events & EPOLLIN) {
                if(!read_once(connfd, buff, 1024)) {
                    close_conn(epollfd, connfd);
                }
                struct epoll_event event;
                event.events = EPOLLOUT | EPOLLET | EPOLLERR;
                event.data.fd = connfd;
                ::epoll_ctl(epollfd, EPOLL_CTL_MOD, connfd, &event);

            } else if(events[i].events & EPOLLOUT) {
                if(!write_bytes(connfd, buff, 1024)) {
                    close_conn(epollfd, connfd);
                }
                struct epoll_event event;
                event.events = EPOLLIN | EPOLLET | EPOLLERR;
                event.data.fd = connfd;
                ::epoll_ctl(epollfd, EPOLL_CTL_MOD, connfd, &event);

            } else if(events[i].events & EPOLLERR) {
                close_conn(epollfd, connfd);
            }
        }

    }

    return 0;
}
