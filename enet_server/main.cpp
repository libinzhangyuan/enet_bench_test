//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <enet/enet.h>

#include "enet_server.h"

int main(int argc, char* argv[])
{
    try
    {
        // Check command line arguments.
        if (argc != 3)
        {
            std::cerr << "Usage: enet_server <address> <port>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    enet_server 0.0.0.0 80\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    enet_server 0::0 80\n";
            return 1;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    std::shared_ptr<EnetServer> server_ptr = std::make_shared<EnetServer>(argv[1], std::atoi(argv[2]));
    server_ptr->Run();

    return 0;
}
