//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_CREAT_SOCKET_H
#define TRASHY_SERVER_CREAT_SOCKET_H
#include "config.h"
#include "tool_util.h"

namespace net {

    class Creat_socket {
    protected:
        sock_t sock_in;
        int sockfd;
    private:
        void init_(sa_family_t family = AF_INET, in_addr_t ip = INADDR_ANY, int opt = 0);
    public:
        Creat_socket();
        Creat_socket(sa_family_t family, rewrite_tool::my_str& ip, int opt);

        ~Creat_socket();
        const addr_in_t get_addr(void) const;
        socklen_t get_addr_len(void) const;
        const int get_sock_fileno(void) const;
    };

} // end of net
#endif //TRASHY_SERVER_CREAT_SOCKET_H
