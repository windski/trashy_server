//
// Created by windsky on 17-8-11.
//

#ifndef TRASHY_SERVER_SERVER_H
#define TRASHY_SERVER_SERVER_H

#include "creat_socket.h"
#include "config.h"

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
	protected:
		int try_open();
	public:
		inline int get_file_handle(void) const { return fileno != 0 ? fileno : 0; }
		GET_Respose(std::string &);
		~GET_Respose();
		void response(int );
	};


    extern void sig_int_Ctrl_c(int sig);


}  // end of namespace net
#endif //TRASHY_SERVER_SERVER_H
