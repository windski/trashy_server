//
// Created by windski on 8/24/17.
// 这个文件用于测试使用..对端程序..客户端..

#include "config.h"
#include "http_request_exm.h"
#include <sys/epoll.h>

int main(int argc, const char *argv[])
{
    int sockfd;
    struct sockaddr_in sockserv;

    memset(&sockserv, 0, sizeof(sockserv));
    sockserv.sin_port = htons(8000);
    sockserv.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &sockserv.sin_addr);

    sockfd  = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv)) == 0) {
        printf("OK\n");
    }

    int n = 0;
    char buff[net::MAXLINE];

    char test_http_header[] = HEAD_REQUEST();

	// read(sockfd, buff, sizeof(buff));
	// printf("%s", buff);
    write(sockfd, test_http_header, sizeof(test_http_header));

    printf("Success to send http header..");
    while((n = read(sockfd, buff, net::MAXLINE)) > 0) {
        printf("read %d bytes form localhost", n);

        printf("\n%s\n", buff);
    }

    return 0;
}
