#include <string>
#include <iostream>

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

    return 0;
}
