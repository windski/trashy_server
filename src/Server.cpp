//
// Created by windsky on 17-8-11.
//

#include "Server.h"
#include "status_code.h"
#include "templates/search_dir.hxx"

namespace net {

    Server::Server(char *ip, int port)
		    : connfd(0)
    {
        rewrite_tool::my_str str(ip);
        Creat_socket(AF_INET, str, 0);
    }

    Server::Server(char *ip)
		    : connfd(0)
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

    void sig_int_Ctrl_c(int sig)
    {
        // TODO: release the memory that alloc
        // 先直接返回.. 后面再说..(埋个坑)
        return ;
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

        void sig_int_Ctrl_c(int);

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
        signal(SIGINT, sig_int_Ctrl_c);
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


	base_Response::base_Response()
			: version(HTTP_VER_STR),
			  server_name(SERVER_NAME),
			  route("index.html")
	{
		http_header_buff = new char[1024];
		memset(http_header_buff, 0, sizeof(http_header_buff));

//		logging(WARN, "config error..");
	}

	base_Response::~base_Response()
	{
		delete[] http_header_buff;
	}

	base_Response::base_Response(std::string &path)
			: version(HTTP_VER_STR),
			  server_name(SERVER_NAME),
			  route(path)
	{
		http_header_buff = new char[1024];
		memset(http_header_buff, 0, sizeof(http_header_buff));

	}


	DEFAULT_Response::DEFAULT_Response()
	: base_Response()
	{

	}

	void DEFAULT_Response::response(int sockfd)
	{
		response_status = 500;

		rewrite_tool::time_hp tm_stp;

		const char *tm_buf = tm_stp.get_time_stamp_net_std1().c_str();

		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\nDate:%s\r\n\r\n",
		        version.c_str(), status_code(response_status), server_name.c_str(),
		        tm_buf);

		std::string tmp(http_header_buff);
		char buff_[net::MAXLINE];

		std::fstream file(route, std::ios::in);
		if(!file.is_open()) {
			logging(WARN, "%s, %d, %s, %s", __FILE__, __LINE__, "can not open file", route.c_str());
		} else {
			while(!file.eof()) {
				file.read(buff_, sizeof(buff_));
			}

			std::string file_storage(buff_);

			tmp += file_storage;
		}

		if(write(sockfd, tmp.c_str(), tmp.size() + 1) != 0) {
			logging(INFO, "%s send fin.", route.c_str());
		} else {
			logging(WARN, "IN %s, %d. from %s can NOT send any pages.", __FILE__, __LINE__, route.c_str());
		}
	}

	DEFAULT_Response::~DEFAULT_Response()
	{

	}


	int GET_Response::try_open()
	{
		config_parse::config_file_parse file_path;
		std::string tmp_path;

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
					// TODO: may delete it, but now...
					tmp_path = file_path.get_pages_path();

					tmp_path += "500.html";

					if((fileno = open(tmp_path.c_str(), O_RDONLY)) < 0) {
						logging(WARN, "Can Not find `500.html`, checkout the file.");
					}
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


	void GET_Response::response(int sockfd)
	{
		ssize_t n = 0;
		ssize_t sum_n = n;
		char buff_[net::MAXLINE];

		try_open();    // get response status code

		std::string file_T__ = get_text_file_T(route);

		logging(DEBUG, "file type is %s", file_T__.c_str());

		rewrite_tool::time_hp tm_stp;

		const char *tm_buf = tm_stp.get_time_stamp_net_std1().c_str();

		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\nDate:%s\r\n\r\n",
		        version.c_str(), status_code(response_status), server_name.c_str(),
		        tm_buf);

		// 未完待续..!!! 到时候把配置脚本模块写出来, 自动判断文本类型..
		// 所以现在http header 少了content/type

		std::string tmp(http_header_buff);
		memset(buff_, 0, sizeof(buff_));

		switch (response_status)
		{
			case 200:
				while((n = read(fileno, (buff_ + sum_n), net::MAXLINE)) > 0) {
					sum_n += n;
					if(sum_n >= net::MAXLINE) {
						logging(WARN, "%s, %d, The file too big", __FILE__, __LINE__);
						break;
					}
				}
				break;
			case 404:
				while((n = read(fileno, (buff_ + sum_n), net::MAXLINE)) > 0) {
					sum_n += n;
					if(sum_n >= net::MAXLINE) {
						logging(WARN, "%s, %d, The file too big", __FILE__, __LINE__);
						break;
					}
				}
				break;
			case 500:
				while((n = read(fileno, (buff_ + sum_n), net::MAXLINE)) > 0) {
					sum_n += n;
					if (sum_n >= net::MAXLINE) {
						logging(WARN, "%s, %d, The file too big", __FILE__, __LINE__);
						break;
					}
				}
				break;
			default:           // The others http status codes
				logging(WARN, "Not send any pages.");
				break;
		}
		// 在insert过程中改变了status_response_str 在内存中的内容, 目测是由于内存布局的原因. 导致被修改,
		// c++ string 就是屌, 已经被cosnt 修饰了, 还能更改...
		// ps. 估计还是对static func 内存布局不熟...
		tmp.insert(tmp.length(), buff_, sum_n);
		if(write(sockfd, tmp.c_str(), tmp.length()) != 0) {
			logging(INFO, "From %s pages send fin.", route.c_str());
		} else {
			logging(ERROR, "From %s can NOT send any pages.", route.c_str());
		}


	}

	GET_Response::~GET_Response()
	{
        close(fileno);            // 对已经调用 close 过的 file descriptor 再调用 close 会置errno 为 EBADF.
	}

	GET_Response::GET_Response(std::string &path)
			: base_Response(path)
	{

	}

	std::string net::GET_Response::get_text_file_T(std::string &str) const
	{
		char parse_buff_[net::MAXLINE];
		memset(parse_buff_, 0, sizeof(parse_buff_));

		strcpy(parse_buff_, str.c_str());
		const char *type_str = strrchr(parse_buff_, '.');

		size_t tail_len = config_parse::str_offset_L(parse_buff_, type_str);
		size_t  tmp_pos__ = str.length();

		if(tmp_pos__ - tail_len > 7)
			logging(WARN, "file's suffix may too long..");

		// TODO: using multimap to storage what file's suffix..
		return str.substr(tail_len + 1, tmp_pos__);
	}

	HEAD_Response::HEAD_Response()
	: base_Response()
	{

	}

	HEAD_Response::HEAD_Response(std::string & path)
	: base_Response(path)
	{

	}

	HEAD_Response::~HEAD_Response()
	{

	}

	void HEAD_Response::response(int sockfd)
	{
		char buff_[net::MAXLINE];
		memset(buff_, 0, sizeof(buff_));

		rewrite_tool::time_hp tm_ntp;
		response_status = 200;

		const char *tm_buf = tm_ntp.get_time_stamp_net_std1().c_str();

		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\nDate:%s\r\n\r\n",
		        version.c_str(), status_code(response_status), server_name.c_str(),
		        tm_buf);

		memcpy(buff_, http_header_buff, sizeof(buff_));
		if(write(sockfd, buff_, sizeof(buff_)) != 0) {
			logging(INFO, "HEAD method send fin..");
		} else {
			logging(ERROR, "HEAD method was NOT send correctly..");
		}
	}


	PUT_Response::PUT_Response(std::string &path)
	:base_Response(path)
	{

	}

	PUT_Response::~PUT_Response()
	{

	}

	void PUT_Response::response(int sockfd)
	{
		char buff_[net::MAXLINE];
		memset(buff_, 0, sizeof(buff_));

		rewrite_tool::time_hp tm_ntp;

		const char *tm_buf = tm_ntp.get_time_stamp_net_std1().c_str();

		try_write();

		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Type: text/html\r\nDate:%s\r\n\r\n",
		        version.c_str(), status_code(response_status), server_name.c_str(),
		        tm_buf);

		if(write(sockfd, http_header_buff, sizeof(http_header_buff)) != 0) {
			logging(INFO, "http response send finish.");
		} else {
			logging(WARN, "socket write error, NOT send http header.");
		}

	}

	void PUT_Response::try_write()
	{

//		rewrite_tool::search_file();

	}
	// TODO: finish the part of try_write

}
