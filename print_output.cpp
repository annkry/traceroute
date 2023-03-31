#include "print_output.h"
#include <iostream>
#include <string>

bool print(std::string received_addresses, int answer, int how_many_distinct_addresses, double RTT, const char *destination)
{
    bool ok = true;
    if (received_addresses != "")
    {
        std::cout << received_addresses;
        if (answer == 3)
        {
            std::cout << RTT / answer << "ms\n";
            if (received_addresses.back() == ' ')
            {
                received_addresses.pop_back();
            }
            if (how_many_distinct_addresses == 1 && received_addresses == destination)
                ok = false;
        }
        else
        {
            std::cout << "???\n";
        }
    }
    else
    {
        std::cout << "*\n";
    }
    return ok;
}