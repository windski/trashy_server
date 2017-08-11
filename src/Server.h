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

    public:
        Server();
        Server(char *, int );
        Server(char *);
        ~Server();
        int config();
        int run();
    };

}  // end of namespace net
#endif //TRASHY_SERVER_SERVER_H
