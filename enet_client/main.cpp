#include <string>
#include <iostream>
#include "enet_client.h"

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: enet_client <connect_to_host> <connect_to_port> <msg_lenth>\n";
            return 1;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize);

    auto client_ptr = std::make_shared<EnetClient>(argv[1], std::atoi(argv[2]), std::atoi(argv[3]));
    client_ptr->Run();

    return 0;
}
