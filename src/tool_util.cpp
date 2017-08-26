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
	memset(http_header_buff, 0, sizeof(http_header_buff));

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
	memset(http_header_buff, 0, sizeof(http_header_buff));

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
        }
        return -1;
    }

    return 0;
}


void rewrite_tool::GET_Respose::response(int sockfd)
{
	ssize_t n = 0;
	ssize_t sum_n = n;
	char buff_[net::MAXLINE];

	try_open();    // get response status code


    sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\n\r\n", version.c_str(), \
    net::status_code(response_status), server_name.c_str());

    // 未完待续..!!! 到时候把配置脚本模块写出来, 自动判断文本类型..
    // 所以现在http header 少了content/type

	std::string tmp(http_header_buff);
	memset(buff_, 0, sizeof(buff_));

    // WTF
    // 突然发现 在发送了http header 之后, 就算是404 还要发送404的页面!!!!

	// 这里其实还有bug, 从文件中读取第二次时有可能覆盖第一次数据..., 不过由于我的测试文件比较小, 没有出现这种现象
    switch (response_status)
    {
        case 200:
	        while((n = read(fileno, (buff_ + sum_n), net::MAXLINE)) > 0) {
		        sum_n += n;
		        if(sum_n > net::MAXLINE) {
			        logging(WARN, "Buff too short OR file too big");
			        break;
		        }
	        }
            break;
        case 404:
	        while((n = read(fileno, (buff_ + sum_n), net::MAXLINE)) > 0) {
		        sum_n += n;
		        if(sum_n > net::MAXLINE) {
			        logging(WARN, "Buff too short OR file too big");
			        break;
		        }
	        }
        default:           // The others http status codes
	        logging(WARN, "Not send any pages.");
            break;
    }
	// 在insert过程中改变了status_response_str 在内存中的内容, 目测是由于内存布局的原因. 导致被修改,
	// c++ string 就是屌, 已经被cosnt 修饰了, 还能更改...
	// ps. 估计还是对static func 内存布局不熟...
	tmp.insert(tmp.length(), buff_, sum_n);
	if(write(sockfd, tmp.c_str(), tmp.length()) != 0) {
		logging(INFO, " From %s pages send fin.", route.c_str());
	} else {
		logging(ERROR, " From %s can NOT send any pages.", route.c_str());
	}


}

rewrite_tool::GET_Respose::~GET_Respose()
{
//    close(fileno);            // 对已经调用 close 过的 file descriptor 再调用 close 会置errno 为 EBADF.
}

rewrite_tool::GET_Respose::GET_Respose(std::string &path)
: base_Respose(path)
{

}
