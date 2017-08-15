//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_MY_STR_H
#define TRASHY_SERVER_MY_STR_H
#include <string>
#include "config.h"
#include <memory>
#include <vector>

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
        enum method { GET, PUT, DELETE, POST, HEAD };
        method request_method;
        std::shared_ptr<std::string> route;

    private:
        int split_(std::vector<std::string>, const std::string &, const std::string);
//        inline int split_(std::pair<std::string, std::string> &, const std::string &, const char);
        void setting_attrib(std::string &source);
    public:
        http_parse(char *);
        http_parse(std::string);

        ~http_parse();
    };

} // end of rewrite_tool
#endif //TRASHY_SERVER_MY_STR_H
