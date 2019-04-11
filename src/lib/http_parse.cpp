//
// Created by acat on 19-4-3.
//

#include "http_parse.h"


namespace core {
namespace parse {


http_parse::http_parse()
: m_httpmethod(nullptr), m_httppath(nullptr)
{
}


const char *http_parse::check_complete(const char *buf, const char *buf_end, int &ret)
{
    int count = 0;

    while(1) {
        CHECK_EOF();
        if(*buf == '\r') {
            buf++;
            CHECK_EOF();
            EXPECT_CHAR('\n');
            count++;
        } else if(*buf == '\n') {
            buf++;
            count++;
        } else {
            buf++;
            count = 0;
        }

        if(count == 2) {
            return buf;
        }
    }
}


int http_parse::parse_request(const char *buf) noexcept
{
    assert(buf != nullptr);
    int slen = strlen(buf);

    int ret = 0;
    // check_complete(buf, buf + slen, ret);


    return 0;
}

http_parse::~http_parse()
{

}


} // end of parse

} // end of core


int main()
{
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
                     "Cache-Control: max-age=0");

    return 0;
}
