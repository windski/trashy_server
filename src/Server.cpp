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
        exit(1);
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
	    signal(SIGINT, sig_int_Ctrl_c);
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

    // TODO: finish POST response
    POST_Response::POST_Response()
    : enctype(unknown_type),
      fdatas(),
      fdatas_pair()
    {

    }

    POST_Response::POST_Response(std::string & _enctype)
    : enctype(unknown_type),
      fdatas(),
      fdatas_pair()
    {
        init_enctype(_enctype);
    }

    POST_Response::POST_Response(std::string & _enctype, std::string & forms)
    : enctype(unknown_type),
      fdatas(forms),
      fdatas_pair()
    {
        init_enctype(_enctype);
    }

    void POST_Response::init_enctype(std::string & _enctype) noexcept
    {
        if(*_enctype.rbegin() == 'd') {
            enctype = urlencode;
        } else if(*_enctype.rbegin() == 'a') {
            enctype = form_data;
        } else if(*_enctype.rbegin() == 'n') {
            enctype = plain;
        } else {
            enctype = unknown_type;
        }
    }

    void POST_Response::response(int sockfd)
    {
        char buff[net::MAXLINE];
        parse_post_form();
    }


    void POST_Response::parse_post_form()
    {
        // TODO: here
//        std::string::size_type lhs, rhs;
//        lhs = 0;

    }

    POST_Response::~POST_Response()
    {

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
	:base_Response(path),
	 request_data(),
	 target_file()
	{

	}


	PUT_Response::PUT_Response(std::string &path, std::string &data)
	: base_Response(path),
	  request_data(data),
	  target_file()
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

		sprintf(http_header_buff, "%s %s\r\n%s\r\nContent-Location:%s\r\nDate:%s\r\n\r\n",
		        version.c_str(), status_code(response_status), server_name.c_str(),
		        route.c_str(), tm_buf);

		if(write(sockfd, http_header_buff, sizeof(http_header_buff)) != 0) {
			logging(INFO, "http response send finish.");
		} else {
			logging(WARN, "socket write error, NOT send http header.");
		}

	}

	void PUT_Response::try_write()
	{
		set_page_data();

		config_parse::config_file_parse file_path;
		std::string search_root = file_path.get_pages_path();

		from_route_get_name(target_file);

		if(rewrite_tool::search_file(search_root, target_file)) {
            try {
                update_page_data(search_root, page_data);
            } catch (std::runtime_error &e) {
                response_status = 500;
                return ;
            }

			response_status = 204;
		} else {
			try {
                update_page_data(search_root, page_data);
            } catch (std::runtime_error &e) {
                response_status = 500;
                return ;
            }
			response_status = 201;
		}

	}

	void PUT_Response::set_page_data()
	{
		std::string::size_type ssize;
		ssize = request_data.rfind("\r\n\r\n");
		page_data = std::move(request_data.substr(ssize, request_data.length()));
	}

	void PUT_Response::from_route_get_name(std::string &target)
	{
		std::string::size_type ssize;
		if(*target.rbegin() != '/') {
			ssize = target.rfind('/');
			std::string && tmp = target.substr(ssize - 1, target.length());
			target.clear();
			target = tmp;
		} else {
			ssize = target.rfind('/');
			std::string && tmp = target.substr(ssize - 1, target.length() - 1);
			target.clear();
			target = tmp;
		}
	}

	int PUT_Response::update_page_data(const std::string & root_dir, const std::string & data) const
	{
		std::string target = root_dir + target_file;
		std::fstream file(target, std::ios::out);

		if(!file.is_open()) {
			logging(ERROR, "Oops! target file %s can't be open.", target.c_str());
			throw std::runtime_error("Target file can't be open..");
		}

        file << data;
        file.close();
        return 0;
	}

}
