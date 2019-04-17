//
// Created by acat on 19-4-3.
//

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include <cassert>
#include <cstring>
#include <memory>

namespace core {

namespace parse {


enum CHECK_STATUS {
    CHKS_HEADER, CHKS_REQUEST_LINE,
};

enum LINE_STATUS {
    LINE_OK, LINE_BAD, LINE_OPEN,
};

enum HTTP_CODE {
    NO_REQUEST, GET_REQUEST, BAD_REQUEST,
    CLOSED_CONNECTION, INTERNAL_ERROR
};


// no-copyable
class http_parse
{
public:
    http_parse();
    ~http_parse();
    HTTP_CODE parse_request(const char *, CHECK_STATUS) noexcept;

private:
    http_parse(const http_parse &);
    http_parse operator=(const http_parse &);

    LINE_STATUS check_line(const char *);
    HTTP_CODE parse_requestline(const char *, CHECK_STATUS);
    HTTP_CODE parse_header(const char *);

    char *m_httpmethod;
    char *m_httppath;

    int m_check_cur;
    int m_start_line;
};




} // end of parse

} // end of core

#endif //HTTP_PARSE_H
