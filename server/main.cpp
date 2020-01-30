#include <iostream>

#include "Server.h"

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./server <serverPort>" << std::endl;
        return 0;
    }

    Server server(atoi(argv[1]));
    server.run();
    return 0;
}
