//
// Created by acat on 19-3-28.
//

#include "service.h"

namespace core {

service::service(int port)
: m_sock(AF_INET, SOCK_STREAM, 0), m_instance()
{
    m_sock.setport(port);
    m_sock.bind();
    net::setnonblocking(m_sock.getfd());
    m_sock.listen();

    m_listen.set<service, &service::listen_accept>(this);
    m_listen.start(m_sock.getfd(), ev::READ);

    m_sig.set<&service::signal_cb>();
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


void service::listen_accept(ev::io &w, int revents) noexcept
{
    if(EV_ERROR & revents) {
        perror("got invalid event");
        return ;
    }

    net::privatedomain::address cliaddr;
    socklen_t socklen = sizeof(struct sockaddr_in);

    int fd = accept(w.fd, (struct sockaddr *)(&cliaddr.getaddr()), &socklen);
    printf("from %s  ", inet_ntoa(cliaddr.getaddr().sin_addr));
    printf("got a peer.\n");

    if(fd < 0) {
        perror("accept error");
        return ;
    }

    auto instance = new privatedomain::serviceinstance(fd);
    assert(instance != nullptr);
    m_instance.push_back(instance);
}


service::~service()
{
    for(auto& i : m_instance) {
        assert(i != nullptr);
        delete i;
    }
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

    if(m_wlist.empty()) {
        m_io.set(ev::READ);
    } else {
        m_io.set(ev::READ | ev::WRITE);
    }
}


void serviceinstance::read_cb(ev::io &w)
{
    char buff[M_MAXSIZE];
    bzero(buff, sizeof(buff));

    ssize_t rsize = recv(w.fd, buff, sizeof(buff), 0);
    if(rsize < 0) {
        perror("read error");
        return ;
    } else if(rsize == 0) {
        delete this;
    } else {
        m_wlist.push_back(new mbuffer(buff, rsize));
    }
}


void serviceinstance::write_cb(ev::io &w) noexcept
{
    if(m_wlist.empty()) {
        m_io.set(ev::READ);
        return ;
    }

    mbuffer *buff = m_wlist.front();
    ssize_t wsize = write(w.fd, buff->dpos(), buff->nbytes());
    if(wsize < 0) {
        perror("write error");
        return ;
    }

    buff->curs += wsize;

    if(buff->nbytes() == 0) {
        m_wlist.pop_front();
        delete buff;
    }
}


serviceinstance::~serviceinstance()
{
    m_io.stop();
    close(m_fd);
}


} // end of privatedomain


} // end of core
