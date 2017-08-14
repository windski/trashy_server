//
// Created by windsky on 17-8-10.
//

#include "tool_util.h"
#include <cstring>

in_addr_t rewrite_tool::my_str::to_addr(net::IP_VERSION ipv) const
{
    char *tmp = nullptr;
    in_addr_t data = 0;
    tmp = const_cast<char *>(c_str());

    if(ipv == net::IPV4) {
        inet_pton(AF_INET, tmp, &data);
    } else if(ipv == net::IPV6) {
        inet_pton(AF_INET6, tmp, &data);
    }
    if(errno != EINVAL) {
        return data;
    }
}

rewrite_tool::my_str::my_str(char *str)
{

}


rewrite_tool::http_parse::http_parse(char * http_str, ssize_t len)
{
    auto splited_str = split_(http_str, len, '\n');       // 将字符串
    // TODO 解析http 协议
}

// TODO 改bug...
std::vector<std::vector<char>> rewrite_tool::http_parse::split_(char *str, ssize_t len, const char s)
{
    std::vector<std::vector<char>> result;
    int i, j;
    for(i = 0; i < len; i = j) {
        char tmp[30];
        for(j = i; j < len; j++) {
            if(str[j] == s) {
                for (int o = i; o <= j; o++) {
                    tmp[o] = str[o];
                }
                break;
            }
        }
        strmem(result[i], tmp, (j - i));  // may be bug here..
        j++;
    }

    return result;
}


int rewrite_tool::http_parse::strmem(std::vector<char> &str, const char *souce, size_t len)
{
    int i;
    for(i = 0; i < len; i++) {
        str.push_back(souce[i]);
    }
    return 0;
}

