#include "config.h"
#include "Server.h"


int main(int argc, char *argv[])
{
    using net::Server;

    Server server;

    server.config();   // 可以钦定IP address 或者 port..

    return server.run();
}
