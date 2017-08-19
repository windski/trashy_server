//
// Created by windsky on 17-8-10.
//

#ifndef TRASHY_SERVER_TOOL_UTIL_H
#define TRASHY_SERVER_TOOL_UTIL_H
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

    extern void setnonblocking(int);

    class http_parse
    {
    private:
        int version;
        enum method { GET, PUT, DELETE, POST, HEAD };
        enum connect_t { KEEP_ALIVE, CLOSE };
        method request_method;
        std::shared_ptr<std::string> route;
        std::string host;
        connect_t request_conn_type;
    private:
        int split_(std::vector<std::string>&, const std::string &, const std::string);
//        inline int split_(std::pair<std::string, std::string> &, const std::string &, const char);
        void setting_attrib(std::string &source);
        void get_method(int);
    public:
        http_parse(char *);
        http_parse(std::string);
        int make_response(int fileno);

        ~http_parse();
    };


    class base_Respose
    {
    private:
        void set_time();
    protected:
        std::string version;
        std::string status_http_code;
        time_t time;
        std::string server_name;

    public:
        base_Respose();
        virtual ~base_Respose();
        virtual void respose(int ) = 0;
    };


    class GET_Respose : public base_Respose
    {
    private:

    public:
        ~GET_Respose();
        void respose(int );
    };



} // end of rewrite_tool
#endif //TRASHY_SERVER_MY_STR_H
