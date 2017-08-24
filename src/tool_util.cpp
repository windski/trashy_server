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



rewrite_tool::base_Respose::base_Respose()
: version(HTTP_VER_STR),
  server_name(SERVER_NAME),
  route("index.html")
{
    http_header_buff = new char[1024];
    logging(WARN, "config error..");
}

rewrite_tool::base_Respose::~base_Respose()
{
    delete[] http_header_buff;
}

rewrite_tool::base_Respose::base_Respose(std::string &path)
: version(HTTP_VER_STR),
  server_name(SERVER_NAME),
  route(path)
{
    http_header_buff = new char[1024];

    if(path == "/" || path == "./") {
        route = path.insert(2, "index.html");
    }
}

int rewrite_tool::GET_Respose::try_open()
{
    if((fileno = open(route.c_str(), O_RDONLY)) > 0) {
        response_status = 200;
    } else {
        switch(errno) {
            case EACCES:
                response_status = 403;
                logging(INFO, "The requested access to the file is not allowed.");
                break;
            case ENXIO:
                response_status = 500;
                logging(ERROR, "The file may is a device file. Or O_NONBLOCK | O_WRONLY is set.");
                break;
            case EEXIST:
                // 300 相关?? 不清楚
                break;
            case ELOOP:
                response_status = 503;
                logging(WARN, "Too Many symbolic links were encountered..");
                break;
            case EFAULT:
                response_status = 500;
                logging(WARN, "`pathname poins outside your accessible address space.`");
                break;
            case EINVAL:
                response_status = 500;
                logging(ERROR, "flags is NOT Suitable...");
                break;
            case EPERM:
                response_status = 500;
                logging(WARN, "The Operation was prevented by a file seal..");
                break;
            default:
                response_status = 404;
                if((fileno = open("./404.html", O_RDONLY)) < 0) {
                    logging(WARN, "Can NOT find `404.html`, checkout the file.");
                }
                break;
                // TODO other http status codes
        }
        return -1;
    }

    return 0;
}


void rewrite_tool::GET_Respose::response(int sockfd) {
    try_open();    // get response status code

    const char *status_response_str = net::status_code(response_status);
    sprintf(http_header_buff, "%s %s\r\n %s\r\n Content-Type: text/html\r\n", version.c_str(), status_response_str,\
     server_name.c_str());
    // 未完待续..!!! 到时候把配置脚本模块写出来, 自动判断文本类型..
    // 所以现在http header 少了content/type

    write(sockfd, http_header_buff, sizeof(http_header_buff));     // http response header

    logging(INFO, "%s Response complete...", status_response_str);

    // WTF
    // 突然发现 在发送了http header 之后, 就算是404 还要发送404的页面!!!!

    ssize_t n = 0;
    char buff_[net::MAXLINE];

    switch (response_status)
    {
        case 200:
            while((n = read(fileno, buff_, net::MAXLINE)) > 0) {
                if(write(sockfd, buff_, n) != n) {
                    logging(ERROR, "Bad write to socket that file descriptor.");
                }
            }
            break;
        case 404:
            while((n = read(fileno, buff_, net::MAXLINE)) > 0) {
                if(write(sockfd, buff_, n) != n) {
                    logging(ERROR, "Bad write to socket that file descriptor.");
                }
            }
        default:           // The others http status codes
            break;
    }

}

rewrite_tool::GET_Respose::~GET_Respose()
{
    close(fileno);            // 对已经调用 close 过的 file descriptor 再调用 close 会置errno 为 EBADF.
}

rewrite_tool::GET_Respose::GET_Respose(std::string &path)
: base_Respose(path)
{

}
