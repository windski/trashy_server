//
// Created by windski on 8/24/17.
// 这个文件用于测试使用..对端程序..客户端..

#include <cstdio>
#include <cstdlib>
#include <error.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

#define MAXQUE 1024
#define MAXLINE 4096

int main(int argc, const char *argv[])
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockserv;

    memset(&sockserv, 0, sizeof(sockserv));
    sockserv.sin_port = htons(23333);
    sockserv.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &sockserv.sin_addr);

    if(bind(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv)) == 0) {
        printf("OK\n");
	}

    listen(sockfd, MAXQUE);

    if(connect(sockfd, (struct sockaddr *)&sockserv, sizeof(sockserv)) == 0) {
        printf("OK\n");
    }

    int n = 0;
    char buff[MAXLINE];

    char test_http_header[] = "GET / HTTP/1.1\r\n Host: localhost:23333\r\n Connect: keep-alive\r\n User-Agent: "
            "Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\n Accept-language: en-US,en;"
            "q=0.8,zh-CN;q=0.6,zh;q=0.4";

	read(sockfd, buff, sizeof(buff));
	printf("%s", buff);
    write(sockfd, test_http_header, sizeof(test_http_header));

    printf("Success to send http header..");
    while((n = read(sockfd, buff, MAXLINE)) > 0) {
        printf("read %d bytes form localhost", n);

        printf("\n%s\n", buff);
    }

    return 0;
}
