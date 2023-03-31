#include "check_if_valid_input.h"
#include <iostream>
#include <string>

bool check_if_valid_input(const char *destination)
{
    std::string part_of_ip_address = "";
    bool valid_input_data = true;
    for (int i = 0; i <= (int)strlen(destination); i++)
    {
        if (destination[i] == '.' || i == (int)strlen(destination))
        {
            if (std::stoi(part_of_ip_address) > 255 || std::stoi(part_of_ip_address) < 0)
            {
                valid_input_data = false;
                break;
            }
            part_of_ip_address = "";
        }
        else if (destination[i] >= 48 && destination[i] <= 57)
        {
            part_of_ip_address += destination[i];
        }
        else
        {
            valid_input_data = false;
            break;
        }
    }
    return valid_input_data;
}