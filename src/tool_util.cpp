//
// Created by windsky on 17-8-10.
//

#include "tool_util.h"
#include "Server.h"
#include <regex>


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
    std::vector<std::string> route_str;
    split_(route_str, splited_str[0], " ");

    route = std::make_shared<std::string>(route_str[1]);

	// 突然..发现, 可以在分割http request 后将其分段标号, 这样就不用在setting_attrib中用丑陋的if-else 串了... ˋ▽ˊ
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
	// TODO: 用switch, 看上面 `突然..` 注释 (´・ω・`)
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

            // TODO POST methods etc. =_=

        default:
            break;
    }

}

void rewrite_tool::http_parse::get_method(int sockfd)
{
    net::GET_Respose G_res((*route).insert(0, "."));
    G_res.response(sockfd);

    // TODO write a script to storage path.

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


