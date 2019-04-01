//
// Created by acat on 19-3-28.
//

#ifndef TRASHY_SERVER_SERVICE_H
#define TRASHY_SERVER_SERVICE_H

#include <ev++.h>
#include "socket.h"
#include <memory>
#include <list>

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

const ssize_t M_MAXSIZE = 1024;


struct mbuffer {
    char *data;
    ssize_t len;
    ssize_t curs;

    mbuffer(const char *bytes, ssize_t nbytes)
        : curs(0), len(nbytes), data(new char[nbytes])
    {
        assert(data != nullptr);
        ::memcpy(data, bytes, nbytes);
    }
    ~mbuffer() { delete [] data; }
    char *dpos() const { return data + curs; }
    ssize_t nbytes() const { return len - curs; }
};


class serviceinstance {
public:
    serviceinstance(int fd);
    void read_cb(ev::io &w);
    void callback(ev::io &w, int revents);
    void write_cb(ev::io &w) noexcept;
    ~serviceinstance();
private:
    int m_fd;
    ev::io m_io;
    std::list<privatedomain::mbuffer *> m_wlist;
};

} // end of privatedomain

}



#endif //TRASHY_SERVER_SERVICE_H
