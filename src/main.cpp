#include <iostream>
#include "config.h"
#include "creat_socket.h"

int main(int argc, char *argv[])
{
    using net::Creat_socket;

    Creat_socket sockfd;

    std::cout << sockfd.get_addr_len() << std::endl;  // test it...

    return 0;
}