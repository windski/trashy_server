//
// Created by acat on 19-3-28.
//

#ifndef TRASHY_SERVER_SERVICE_H
#define TRASHY_SERVER_SERVICE_H

#include <ev++.h>
#include "socket.h"

namespace core {

class service
{
private:
    ev::io m_listen;
    ev::sig m_sig;
    net::socket m_sock;
    ev::default_loop m_loop;

public:
    explicit service(int port);
    ~service();
    void listen_accept(ev::io &w, int revents);
    static void signal_cb(ev::sig &signal, int revents);

    void run();
};


namespace privatedomain {

class serviceinstance {
public:
    serviceinstance(int fd);
    void read_cb(ev::io &w);
    void callback(ev::io &w, int revents);
    void write_cb(ev::io &w);
    ~serviceinstance();
private:
    int m_fd;
    ev::io m_io;

};

} // end of privatedomain

}



#endif //TRASHY_SERVER_SERVICE_H
