//
// Created by windsky on 17-8-10.
//

#include "tool_util.h"
#include "status_code.h"
#include <regex>
#include <netinet/in.h>

in_addr_t rewrite_tool::my_str::to_addr(net::IP_VERSION ipv) const
{
    char *tmp = nullptr;
    in_addr_t data = 0;
    tmp = const_cast<char *>(c_str());

    if(ipv == net::IPV4) {
        inet_pton(AF_INET, tmp, &data);
    } else if(ipv == net::IPV6) {
        inet_pton(AF_INET6, tmp, &data);
    }
    if(errno != EINVAL) {
        return data;
    }
}

rewrite_tool::my_str::my_str(char *str)
{

}


rewrite_tool::http_parse::http_parse(char * http_str)
{
    std::string tmp_str(http_str);
    std::vector<std::string> splited_str;
    split_(splited_str, tmp_str, "\r\n");       // 将字符串以\r\n分割
    // TODO 解析http 协议
    std::vector<std::string> route_str;
    split_(route_str, splited_str[0], " ");

    route = std::make_shared<std::string>(route_str[1]);

    for(auto i : splited_str) {
        setting_attrib(i);
    }

}

rewrite_tool::http_parse::http_parse(std::string)
{

}

int rewrite_tool::http_parse::split_(std::vector<std::string>&result, const std::string &source, const std::string s)
{
    std::string::size_type rhs, lhs;
    lhs = 0;
    rhs = source.find(s);

    while(std::string::npos != rhs) {
        result.push_back(source.substr(lhs, (rhs - lhs)));

        lhs = rhs + s.size();
        rhs = source.find(s, lhs);
    }
//    if(lhs != source.length()) {
//        result.push_back(s.substr(lhs));
//    }

    return 0;
}

rewrite_tool::http_parse::~http_parse()
{

}

void rewrite_tool::http_parse::setting_attrib(std::string &source)
{
    if(std::regex_search(source, std::regex("(GET)"))) {
        request_method = GET;
    } else if(std::regex_search(source, std::regex("(POST)"))) {
        request_method = POST;
    } else if(std::regex_search(source, std::regex("(PUT)"))) {
        request_method = PUT;
    } else if(std::regex_search(source, std::regex("(DELETE)"))) {
        request_method = DELETE;
    } else if(std::regex_search(source, std::regex("(HEAD)"))) {
        request_method = HEAD;
    } else if(std::regex_search(source, std::regex("(HTTP/1.1)"))) {
        version = 1;
    } else if(std::regex_search(source, std::regex("(Host:)"))) {
        std::string::size_type index = source.find("Host");
        host = source.substr(index + 5);
    }
}

int rewrite_tool::http_parse::make_response(int fileno)
{
    switch(request_method) {
        case GET:
            get_method(fileno);
            break;

            // TODO POST methods etc.

        default:
            break;
    }

}

void rewrite_tool::http_parse::get_method(int sockfd)
{
    GET_Respose G_res((*route).insert(0, "."));
    G_res.response(sockfd);
//    (*route).insert(0, ".");    // 先用这种解决办法吧..233
//    // TODO write a script to storage path.
//    char buff[net::MAXLINE];
//    ssize_t n;
//    std::string tmp;
//    int status_code = 0;
//
//    if((fileno = open((*route).c_str(), O_RDONLY)) > 0) {
//        status_code = 200;
//        // 感觉还是需要一个字符串的拼接函数...
//        // 恩....懒...
//        tmp = std::string("HTTP/1.1 200 OK\r\n Connection: close\r\n Server:Demo Server/2.3.3(Debian OS)\r\n Content-Type: text/html\r\n\r\n");
//    } else {
//        switch(errno) {
//            case EACCES:
//                // 403 forbidden
//                break;
//            case ENXIO:
//                perror("O_NONBLOCK flag has been set");
//                status_code = 500;
//                close(sockfd);
//                break;
//            default:
//                status_code = 404;
//                tmp = std::string("HTTP/1.1 404 Not Found\r\n Connection: close\r\n Server:Demo Server/2.3.3(Debian OS)\r\n Content-Type: text/html\r\n\r\n");
//                break;
//        }
//    }
//
//    write(sockfd, tmp.c_str(), tmp.size());
//
//    if(status_code == 200) {
//        while ((n = read(fileno, buff, net::MAXLINE)) > 0)
//            if (write(sockfd, buff, n) != n) {
//                perror("write error");
//            }
//    }
//
    close(sockfd);
}

void rewrite_tool::setnonblocking(int fileno)
{
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



rewrite_tool::base_Respose::base_Respose()
: version(HTTP_VER_STR),
  server_name(SERVER_NAME),
  route("index.html")
{
    *buff = new char[100];
}

rewrite_tool::base_Respose::~base_Respose()
{

}

rewrite_tool::base_Respose::base_Respose(std::string &path)
: version(HTTP_VER_STR),
  server_name(SERVER_NAME),
  route(path)
{
    *buff = new char[100];

    if(path == "/" || path == "./") {
        route = "index.html";
    }
}

int rewrite_tool::base_Respose::try_open()
{
    if((fileno = open(route.c_str(), O_RDONLY)) > 0) {
        status_http_code = 200;
    } else {
        local_errno = errno;
        return -1;
    }

    return 0;
}


void rewrite_tool::GET_Respose::response(int sockfd)
{
    if(try_open() == 0) {
        // 200 response
        sprintf(*buff, "%s %s\r\n %s\r\n\r\n", version, net::status_code(status_http_code), server_name);  // 未完待续..!!!
        logging(INFO, "200 Response");
    } else {
        // TODO other http status codes
    }

    write(sockfd, *buff, sizeof(buff));     // http response header

}

rewrite_tool::GET_Respose::~GET_Respose()
{
    close(fileno);
}

rewrite_tool::GET_Respose::GET_Respose(std::string &path)
: base_Respose(path)
{

}
