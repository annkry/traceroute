#include <netinet/ip.h>
#include <iostream>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <set>
#include "icmpchecksum.h"
#include "check_if_valid_input.h"
#include "receive.h"
#include "send.h"
#include "print_output.h"

int main(int argc, char **argv)
{
    const char *destination;
    if (argc == 2)
    {
        destination = argv[1];
    }
    // check if input data is valid
    if (!check_if_valid_input(destination))
    {
        fprintf(stderr, "input data error: %s\n", "Not a proper ip.");
        return EXIT_FAILURE;
    }
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    int ttl = 1;
    bool ok = true;
    int nr_package = 0;
    std::set<int> ip_seq_set;
    std::set<int> ip_id_set;
    std::vector<std::pair<int, timeval>> time_frame; //<nr_package, send_time>
    int how_many_responses_per_ttl;
    int how_many_distinct_addresses;
    double RTT;
    struct timeval send_time;
    int unique_id;
    int exit_code;
    std::string received_addresses;
    std::pair<int, std::vector<std::pair<std::string, double>>> receive_all;
    while (ok && ttl <= 30)
    {
        exit_code = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
        if (exit_code == EXIT_FAILURE)
        {
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        ip_id_set.clear();
        ip_seq_set.clear();
        time_frame.clear();
        how_many_responses_per_ttl = 0;
        if (ttl < 10)
        {
            std::cout << " ";
        }
        std::cout << ttl << ". ";
        for (int i = 0; i < 3; i++)
        {
            struct timeval unique;
            gettimeofday(&unique, NULL);
            unique_id = (int)(unique.tv_sec % 100000) + (int)(unique.tv_usec);
            nr_package += 1;
            exit_code = send(unique_id % 9999, sockfd, ttl, nr_package, destination, send_time);
            if (exit_code == EXIT_FAILURE)
            {
                fprintf(stderr, "Error: %s\n", "From the function send.");
                return EXIT_FAILURE;
            }
            time_frame.push_back(std::make_pair(nr_package, send_time));
            ip_id_set.insert(unique_id % 9999 + ttl + nr_package);
            ip_seq_set.insert(nr_package);
        }
        // start receiving
        receive_all = receive(sockfd, ip_id_set, ip_seq_set, time_frame);
        if (receive_all.first == EXIT_FAILURE)
        {
            fprintf(stderr, "Error: %s\n", "From the function receive.");
            return EXIT_FAILURE;
        }
        received_addresses = receive_all.second[0].first;
        how_many_responses_per_ttl = (int)receive_all.second[0].second;
        how_many_distinct_addresses = std::stoi(receive_all.second[1].first);
        RTT = receive_all.second[1].second;
        // start printing the results
        ok = print(received_addresses, how_many_responses_per_ttl, how_many_distinct_addresses, RTT, destination);
        ttl += 1;
    }
    return EXIT_SUCCESS;
}
