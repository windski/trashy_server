//
// Created by falleuion on 8/19/17.
//

#ifndef TRASHY_SERVER_LOG_H
#define TRASHY_SERVER_LOG_H

#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <string>
#include <cstdlib>
#include <unistd.h>

// Just write some functions. The functions was based on printf.
// forget that..

enum LEVEL { DEBUG, INFO, WARN, ERROR };


/**
 * ctime 返回的字符串有点迷! 才醒悟...尴尬, 最后的非打印字符是\n\0 ,下面是strlen 的标准库实现
 *
 * size_t __cdecl strlen (const char * str)
 * {
 *      const char *eos = str;
 *
 *      while( *eos++ );
 *
 *      return( eos - str - 1 );
 * }
 */

#define DEFAULT_MAXLENGTH 4096

#define format_time_str(fmt_time_str)                                       \
int format_time_str_len = strlen((fmt_time_str));                           \
fmt_time_str[format_time_str_len - 1] = '\0'

#define logging_in(level, logging_str)                                                             \
time_t log_time = time(NULL);                                                                      \
char *logging_tmp_str = ctime(&log_time);                                                          \
format_time_str(logging_tmp_str);                                                                  \
switch(level) {                                                                                    \
case DEBUG:                                                                                        \
    printf("[%s] %s , %s\n", "DEBUG", logging_tmp_str, (logging_str));break;                       \
case INFO:                                                                                         \
    printf("[%s] %s , %s\n", "INFO", logging_tmp_str, (logging_str));break;                        \
case WARN:                                                                                         \
    printf("[%s] %s , %s\n", "WARN", logging_tmp_str, (logging_str));break;                        \
case ERROR:                                                                                        \
    char logging_char_buff[DEFAULT_MAXLENGTH];                                                     \
    memset(logging_char_buff, 0, sizeof(logging_char_buff));                                       \
    sprintf(logging_char_buff, "[%s] %s , %s\n", "ERROR", logging_tmp_str, (logging_str));         \
    fputs(logging_char_buff, stderr);break;                                                        \
default:break;                                                                                     \
}                                                                                                  \
printf("")

// macro 太TMD丑了...

static void logging(LEVEL level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    std::string fmt_str(fmt);

    for(int i = 0; i < fmt_str.length(); i++) {
        if(fmt_str[i] != '%') {
            continue;
        }

        i++;

        // 不要问这里为什么使用if-else 而不用switch
        // 这里不知不觉的成为了魔幻代码, 不过跟屎一样倒是真的...
        if(fmt_str[i] == 's') {
            std::string str_l = fmt_str.substr(0, i - 1);
            std::string str_r = fmt_str.substr(i + 1, fmt_str.length());
            char *p_str = va_arg(args, char *);
            str_l.insert(str_l.length(), p_str);
            str_l += str_r;
            fmt_str.clear();
            fmt_str = std::move(str_l);
        } else if(fmt_str[i] == '%') {
            continue;
        } else if(fmt_str[i] == 'd') {
            int num__ = va_arg(args, int);
            char result_[7];
            sprintf(result_, "%d", num__);
            std::string str_l = fmt_str.substr(0, i - 1);
            std::string std_r = fmt_str.substr(i + 1, fmt_str.length());
            str_l.insert(str_l.length(), result_);
            str_l += std_r;
            fmt_str.clear();
            fmt_str = std::move(str_l);
        }

    }

    logging_in(level, fmt_str.c_str());
}


#endif //TRASHY_SERVER_LOG_H
