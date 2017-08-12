//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_MY_STR_H
#define TRASHY_SERVER_MY_STR_H
#include <string>
#include "config.h"

namespace rewrite_tool {

    class my_str : public std::string {
    public:
        my_str(char *str);
        in_addr_t to_addr(net::IP_VERSION ipv) const;
    };

} // end of rewrite_tool
#endif //TRASHY_SERVER_MY_STR_H
