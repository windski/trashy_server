//
// Created by windsky on 17-8-11.
//

#include "Server.h"
#include "status_code.h"

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

                    rewrite_tool::setnonblocking(connfd);

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


    void Server::do_somthing(int fileno)
    {
        char buff[MAXLINE];
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
        parse.make_response(fileno);

    }


	base_Respose::base_Respose()
			: version(HTTP_VER_STR),
			  server_name(SERVER_NAME),
			  route("index.html")
	{
		http_header_buff = new char[1024];
		memset(http_header_buff, 0, sizeof(http_header_buff));

		logging(WARN, "config error..");
	}

	base_Respose::~base_Respose()
	{
		delete[] http_header_buff;
	}

	base_Respose::base_Respose(std::string &path)
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

	int GET_Respose::try_open()
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


	void GET_Respose::response(int sockfd)
	{
		ssize_t n = 0;
		ssize_t sum_n = n;
		char buff_[net::MAXLINE];

		try_open();    // get response status code


		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\n\r\n", version.c_str(), status_code(response_status), server_name.c_str());

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

	GET_Respose::~GET_Respose()
	{
//    close(fileno);            // 对已经调用 close 过的 file descriptor 再调用 close 会置errno 为 EBADF.
	}

	GET_Respose::GET_Respose(std::string &path)
			: base_Respose(path)
	{

	}


}