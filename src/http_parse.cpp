//
// Created by acat on 19-4-3.
//

#include "http_parse.h"
#include <cstring>
#include <iostream>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <absl/hash/hash.h>

namespace core {
namespace parse {


http_parse::http_parse()
: m_httpmethod(UNKNOWN), m_httppath(nullptr),
    m_check_cur(0), m_start_line(0)
{
}

HTTP_CODE http_parse::parse_request(const char *content, CHECK_STATUS status) noexcept
{
    LINE_STATUS line = LINE_OK;
    HTTP_CODE retcode = NO_REQUEST;

    while((line = check_line(content)) == LINE_OK) {
        const char *tmp = content + m_start_line;
        m_start_line = m_check_cur;

        switch(status) {
            case CHKS_HEADER: {
                retcode = parse_requestline(tmp, status);
                if(retcode == BAD_REQUEST) {
                    return BAD_REQUEST;
                }
                break;
            }
            case CHKS_REQUEST_LINE: {
                retcode = parse_header(tmp);
                if(retcode == BAD_REQUEST) {
                    return BAD_REQUEST;
                } else if(retcode == GET_REQUEST) {
                    return GET_REQUEST;
                }

                break;
            }
            default: {
                return INTERNAL_ERROR;
            }
        }
    }

    if(line == LINE_OPEN) {
        return NO_REQUEST;
    } else {
        return BAD_REQUEST;
    }
}


LINE_STATUS http_parse::check_line(const char *buff)
{
    char tmp;

    int end_cur = strlen(buff);
    for(; m_check_cur < end_cur; m_check_cur++) {
        tmp = buff[m_check_cur];
        if(tmp == '\r') {
            // the `m_check_cur' is pointer to last character at the point.
            if(m_check_cur + 1 == end_cur) {
                return LINE_OPEN;
            } else if(m_check_cur + 1 == '\n') {
                m_check_cur += 2;
                return LINE_OK;
            }

            return LINE_BAD;
        } else if(tmp == '\n') {
            if((m_check_cur > 1) && buff[m_check_cur - 1] == '\r') {
                m_check_cur += 2;
                return LINE_OK;
            }

            return LINE_BAD;
        }
    }

    return LINE_OPEN;
}


// TODO: try to implement
HTTP_CODE http_parse::parse_requestline(const char *buff, CHECK_STATUS status)
{
    assert(buff != nullptr);

    std::string tbuff(buff);
    auto idx = tbuff.find(" \t");
    if(idx == std::string::npos) {
        return BAD_REQUEST;
    }
    tbuff[idx++] = '\0';



}


HTTP_CODE http_parse::parse_header(const char *buff)
{

}


http_parse::~http_parse()
{

}


} // end of parse

} // end of core


int main()
{
    core::parse::CHECK_STATUS status = core::parse::CHKS_REQUEST_LINE;
    core::parse::http_parse ht;
    ht.parse_request("GET /home.html HTTP/1.1\r\n"
                     "Host: developer.mozilla.org\r\n"
                     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                     "Accept-Language: en-US,en;q=0.5\r\n"
                     "Accept-Encoding: gzip, deflate, br\r\n"
                     "Referer: https://developer.mozilla.org/testpage.html\r\n"
                     "Connection: keep-alive\r\n"
                     "Upgrade-Insecure-Requests: 1\r\n"
                     "If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT\r\n"
                     "If-None-Match: \"c561c68d0ba92bbeb8b0fff2a9199f722e3a621a\"\r\n"
                     "Cache-Control: max-age=0\r\n\r\n", status);

    return 0;
}
