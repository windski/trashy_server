//
// Created by windski on 8/24/17.
// 这个文件用于测试使用..对端程序..客户端..

#include "../src/config.h"

int main(int argc, const char *argv[])
{
    if(argc != 2) {
        logging(ERROR, "usage: %s address", argv[0]);
        exit(-1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockserv;

    memset(&sockserv, 0, sizeof(sockserv));
    sockserv.sin_port = htons(DEBUG_PORT);
    sockserv.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &sockserv.sin_addr);

    bind(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv));

    listen(sockfd, net::MAXQUE);

    connect(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv));

    int n = 0;
    char buff[net::MAXLINE];

    char test_http_header[] = "GET / HTTP/1.1\r\n Host: localhost:23333\r\n Connect: keep-alive\r\n User-Agent: "
            "Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\n Accept-language: en-US,en;"
            "q=0.8,zh-CN;q=0.6,zh;q=0.4";

    write(sockfd, test_http_header, sizeof(test_http_header));

    logging(INFO, "Success to send http header..");
    while((n = read(sockfd, buff, net::MAXLINE)) > 0) {
        logging(INFO, "read %d bytes form localhost", n);

        printf("\n%s\n", buff);
    }

    return 0;
}