//
// Created by acat on 19-4-3.
//

#include "http_parse.h"


namespace core {
namespace parse {


http_parse::http_parse()
: m_httpmethod(nullptr), m_httppath(nullptr),
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


// TODO: try to implement
LINE_STATUS http_parse::check_line(const char *)
{

}


HTTP_CODE http_parse::parse_requestline(const char *, CHECK_STATUS)
{

}


HTTP_CODE http_parse::parse_header(const char *)
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
    ht.parse_request("GET /home.html HTTP/1.1\n"
                     "Host: developer.mozilla.org\n"
                     "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\n"
                     "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n"
                     "Accept-Language: en-US,en;q=0.5\n"
                     "Accept-Encoding: gzip, deflate, br\n"
                     "Referer: https://developer.mozilla.org/testpage.html\n"
                     "Connection: keep-alive\n"
                     "Upgrade-Insecure-Requests: 1\n"
                     "If-Modified-Since: Mon, 18 Jul 2016 02:36:04 GMT\n"
                     "If-None-Match: \"c561c68d0ba92bbeb8b0fff2a9199f722e3a621a\"\n"
                     "Cache-Control: max-age=0", status);

    return 0;
}
