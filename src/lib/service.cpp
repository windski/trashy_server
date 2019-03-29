//
// Created by acat on 19-3-28.
//

#include "service.h"

namespace core {

service::service()
// : m_loop(ev_default_loop(0))
{

}

void service::run()
{
    ::ev_loop(m_loop, 0);
}


service::~service()
{
    // net::~socket();
}


} // end of core

