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


	class base_Respose
	{
	protected:
		std::string version;
		std::string server_name;
		char * http_header_buff;
		std::string route;
		size_t response_status;

	public:
		base_Respose();
		explicit base_Respose(std::string &);
		virtual ~base_Respose();
		virtual void response(int ) = 0;
	};


	class GET_Respose : public base_Respose
	{
	private:
		int fileno;
		std::string file_type;
	protected:
		int try_open();
		std::string get_text_file_T(std::string & str) const;
	public:
		inline int get_file_handle(void) const { return fileno != 0 ? fileno : 0; }
		GET_Respose(std::string &);
		~GET_Respose();
		void response(int );
	};


	class HEAD_Respose : public base_Respose
	{
	public:
		HEAD_Respose();
		explicit HEAD_Respose(std::string &);
		~HEAD_Respose();
		void response(int );

	};


	class PUT_Response : public base_Respose
	{
	private:
		void try_write();
	public:
		PUT_Response(std::string &);
		~PUT_Response();
		void response(int );
	};

    extern void sig_int_Ctrl_c(int sig);


}  // end of namespace net
#endif //TRASHY_SERVER_SERVER_H
