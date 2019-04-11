//
// Created by acat on 19-4-3.
//

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include <cassert>
#include <cstring>

namespace core {

namespace parse {

#define CHECK_EOF()         \
    if(buf == buf_end) {    \
        ret = -1;           \
        return nullptr;     \
    }

#define EXPECT_CHAR(ch)     \
    CHECK_EOF();            \
    if(*buf++ != (ch)) {    \
        ret = -2;           \
        return nullptr;     \
    }

// no-copyable
class http_parse
{
public:
    http_parse();
    ~http_parse();
    int parse_request(const char *) noexcept;

private:
    http_parse(const http_parse &);
    http_parse operator=(const http_parse &);
    char *find_token(const char *buf, const char *token);

    const char *check_complete(const char *buf, const char *buf_end, int &ret);
    char *m_httpmethod;
    char *m_httppath;


};




} // end of parse

} // end of core

#endif //HTTP_PARSE_H
