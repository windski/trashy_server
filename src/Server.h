//
// Created by windsky on 17-8-11.
//

#ifndef TRASHY_SERVER_SERVER_H
#define TRASHY_SERVER_SERVER_H

#include "creat_socket.h"
#include "config.h"
#include "script_parse.h"

namespace net {

    class Server : private Creat_socket {
    private:
        int connfd;
        void do_somthing(int);
    public:
        Server();
        Server(char *, int );
        Server(char *);
        ~Server();
        int config();
        int run();
    };


	class base_Response : private rewrite_tool::utils::Uncopyable
	{
	protected:
		std::string version;
		std::string server_name;
		char * http_header_buff;
		std::string route;
		size_t response_status;

	public:
		base_Response();
		explicit base_Response(std::string &);
		virtual ~base_Response();
		virtual void response(int ) = 0;
	};


	class DEFAULT_Response : public base_Response
	{
	public:
		DEFAULT_Response();
		~DEFAULT_Response();
		void response(int);
	};


	class GET_Response : public base_Response
	{
	private:
		int fileno;
		std::string file_type;
	protected:
		int try_open();
		std::string get_text_file_T(std::string & str) const;
	public:
		inline int get_file_handle(void) const { return fileno != 0 ? fileno : 0; }
		GET_Response(std::string &);
		~GET_Response();
		void response(int );
	};


    class POST_Response : public base_Response
    {
    private:
        enum enc_t{ urlencode, form_data, plain, unknown_type };
        enc_t enctype;
        std::vector<std::pair<std::string, std::string>> fdatas_pair;
        std::string fdatas;

    private:
        void parse_post_form();
        void init_enctype(std::string &) noexcept;

    public:
        POST_Response();
        POST_Response(std::string &);
        POST_Response(std::string &, std::string &);
        ~POST_Response();
        void response(int );

    };


	class HEAD_Response : public base_Response
	{
	public:
		HEAD_Response();
		explicit HEAD_Response(std::string &);
		~HEAD_Response();
		void response(int );

	};


	class PUT_Response : public base_Response
	{
	private:
		std::string request_data;
		std::string page_data;
		std::string target_file;
	private:
		void try_write();
		void set_page_data();
		void from_route_get_name(std::string &);
		int update_page_data(const std::string &, const std::string &) const;
	public:
		explicit PUT_Response(std::string &);
		PUT_Response(std::string &, std::string &);
		~PUT_Response();
		void response(int );
	};

    extern void sig_int_Ctrl_c(int sig);


}  // end of namespace net
#endif //TRASHY_SERVER_SERVER_H
