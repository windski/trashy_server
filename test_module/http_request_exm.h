#ifndef HTTP_REQUEST_EXM_H_
#define HTTP_REQUEST_EXM_H_

#define HEAD_REQUEST() "HEAD / HTTP/1.1\r\n\r\n"

#define GET_REQUEST()  "GET / HTTP/1.1\r\n Host: localhost:23333\r\n Connect: keep-alive\r\n User-Agent: \
    Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0\r\n Accept-language: en-US,en;  \
    q=0.8,zh-CN;q=0.6,zh;q=0.4\r\n\r\n" 

#define PUT_REQUEST(data) PUT /new.html HTTP/1.1 \r\n Host: example.com \r\n Content-type: text/html \r\n\
    Content-length: 16 \r\n\r\n ##data

#define POST_REQUEST() 

#define DELETE_REQUEST() 



#endif
