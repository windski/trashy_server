//
// Created by acat on 19-3-28.
//

#include "service.h"

namespace core {

service::service(int port)
: m_sock(AF_INET, SOCK_STREAM, 0)
{
    m_sock.setport(port);
    m_sock.bind();
    net::setnonblocking(m_sock.getfd());
    m_sock.listen();

    m_listen.set<service, &service::listen_accept>(this);
    m_listen.start(m_sock.getfd(), ev::READ);

    m_sig.set<&service::signal_cb>(this);
    m_sig.start(SIGINT);
}

void service::run()
{
    m_loop.run(0);
}


void service::signal_cb(ev::sig &signal, int revents)
{
    signal.loop.break_loop();
}


void service::listen_accept(ev::io &w, int revents)
{
    if(EV_ERROR & revents) {
        perror("got invalid event");
        return ;
    }

    net::privatedomain::address cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);

    int fd = accept(w.fd, (struct sockaddr *)(&cliaddr.getaddr()), &socklen);

    if(fd < 0) {
        perror("accept error");
        return ;
    }

    // TODO: fix
    auto instance = new privatedomain::serviceinstance(fd);
}


service::~service()
{
}


namespace privatedomain{

serviceinstance::serviceinstance(int fd)
: m_fd(fd)
{
    net::setnonblocking(m_fd);
    m_io.set<serviceinstance, &serviceinstance::callback>(this);
    m_io.start(fd, ev::READ);
}

void serviceinstance::callback(ev::io &w, int revents)
{
    if(EV_ERROR & revents) {
        perror("got invalid events");
        return ;
    }

    if(revents & ev::READ) {
        read_cb(w);
    }

    if(revents & ev::WRITE) {
        write_cb(w);
    }

    // TODO: need a buffer.
}


void serviceinstance::read_cb(ev::io &w)
{

}

void serviceinstance::write_cb(ev::io &w)
{

}


serviceinstance::~serviceinstance()
{
    m_io.stop();
    close(m_fd);
}


} // end of privatedomain


} // end of core


