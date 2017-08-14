//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_MY_STR_H
#define TRASHY_SERVER_MY_STR_H
#include <string>
#include "config.h"
#include <memory>
#include <vector>
#include <map>

namespace rewrite_tool {

    class my_str : public std::string {
    public:
        my_str(char *str);
        in_addr_t to_addr(net::IP_VERSION ipv) const;
    };

    class http_parse
    {
    private:
        int version;
        enum method { NONE, GET, PUT, DELETE, POST, HEAD };
        method request_method;
        std::shared_ptr<char> route;

    private:
        std::vector<std::vector<char>>split_(char *, ssize_t, const char);
        inline int strmem(std::vector<char> &, const char *, size_t);
    public:
        http_parse(char *, ssize_t);
        http_parse(std::string);

        ~http_parse();
    };

} // end of rewrite_tool
#endif //TRASHY_SERVER_MY_STR_H
