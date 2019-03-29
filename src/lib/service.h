//
// Created by acat on 19-3-28.
//

#ifndef TRASHY_SERVER_SERVICE_H
#define TRASHY_SERVER_SERVICE_H

#include <ev.h>
#include "socket.h"

namespace core {

class service
{
private:
    struct ev_loop *m_loop;
    struct ev_io *m_listen;


public:
    service();
    ~service();

    void run();
};

}



#endif //TRASHY_SERVER_SERVICE_H
