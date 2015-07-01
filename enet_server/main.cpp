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

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 3)
    {
      std::cerr << "Usage: enet_server <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    battle_server 0.0.0.0 80\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    battle_server 0::0 80\n";
      return 1;
    }

  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
