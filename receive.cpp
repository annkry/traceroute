#include "receive.h"
#include <netinet/ip_icmp.h>
#include <cstring>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <iostream>

struct timeval return_send_time(std::vector<std::pair<int, timeval>> time_frame, int seq)
{
    struct timeval send_time;
    for (int k = 0; k < 3; k++)
    {
        if (time_frame[k].first == seq)
        {
            send_time = time_frame[k].second;
        }
    }
    return send_time;
}

void compute_time(std::string ip, int seq, std::set<std::string> &ip_addresses_response, int &how_many_responses_per_ttl, timeval &receive_time, double &RTT, std::vector<std::pair<int, timeval>> &time_frame)
{
    struct timeval send_time = return_send_time(time_frame, seq);
    how_many_responses_per_ttl += 1;
    ip_addresses_response.insert(ip);
    RTT += (receive_time.tv_sec - send_time.tv_sec) * 1000.0 + (receive_time.tv_usec - send_time.tv_usec) / 1000.0;
}

std::pair<int, std::vector<std::pair<std::string, double>>> receive(int sockfd, std::set<int> ip_id_set, std::set<int> ip_seq_set, std::vector<std::pair<int, timeval>> time_frame)
{
    std::vector<std::pair<std::string, double>> result;
    std::set<std::string> ip_addresses_response;
    double RTT = 0;
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    int how_many_responses_per_ttl = 0;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
    struct timeval receive_time;
    const char *res;
    ssize_t packet_len;
    struct ip *first_ip_header;
    ssize_t first_ip_header_len;
    ssize_t first_icmp_header_len;
    struct ip *original_ip_header;
    ssize_t original_ip_header_len;
    struct icmp *original_icmp_header;
    struct ip *ip_header;
    ssize_t ip_header_len;
    struct icmp *icmp_header;
    struct sockaddr_in sender;
    socklen_t sender_len;
    u_int8_t buffer[1048];
    char sender_ip_str[20];
    if (ready < 0)
    {
        fprintf(stderr, "select error: %s\n", strerror(errno));
        return std::make_pair(EXIT_FAILURE, std::vector<std::pair<std::string, double>>());
    }
    else if (ready == 1)
    {
        sender_len = sizeof(sender);
        while (how_many_responses_per_ttl != 3 && ready > 0)
        {
            gettimeofday(&receive_time, nullptr);
            packet_len = recvfrom(
                sockfd,
                buffer,
                1048,
                0,
                (struct sockaddr *)&sender,
                &sender_len);
            if (packet_len < 0)
            {
                fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
                return std::make_pair(EXIT_FAILURE, std::vector<std::pair<std::string, double>>());
            }
            while (packet_len >= 0)
            {
                res = inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
                if (res == NULL)
                {
                    fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
                    return std::make_pair(EXIT_FAILURE, std::vector<std::pair<std::string, double>>());
                }
                ip_header = (struct ip *)buffer;
                ip_header_len = 4 * ip_header->ip_hl;
                icmp_header = (struct icmp *)(buffer + ip_header_len);
                if (icmp_header->icmp_type == ICMP_ECHOREPLY && ip_id_set.count(icmp_header->icmp_id) && ip_seq_set.count(icmp_header->icmp_seq))
                {
                    compute_time(sender_ip_str, icmp_header->icmp_seq, ip_addresses_response, how_many_responses_per_ttl, receive_time, RTT, time_frame);
                }
                else if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED)
                {
                    first_ip_header = (struct ip *)(buffer);
                    first_ip_header_len = 4 * first_ip_header->ip_hl;
                    first_icmp_header_len = 8;
                    original_ip_header = (struct ip *)(buffer + first_ip_header_len + first_icmp_header_len);
                    original_ip_header_len = 4 * original_ip_header->ip_hl;
                    original_icmp_header = (struct icmp *)(buffer + first_ip_header_len + first_icmp_header_len + original_ip_header_len);
                    if (ip_id_set.count(original_icmp_header->icmp_id) && ip_seq_set.count(original_icmp_header->icmp_seq))
                    {
                        compute_time(sender_ip_str, original_icmp_header->icmp_seq, ip_addresses_response, how_many_responses_per_ttl, receive_time, RTT, time_frame);
                    }
                }
                gettimeofday(&receive_time, nullptr);
                packet_len = recvfrom(
                    sockfd,
                    buffer,
                    1048,
                    MSG_DONTWAIT,
                    (struct sockaddr *)&sender,
                    &sender_len);
            }
            if (how_many_responses_per_ttl != 3 && (tv.tv_sec != 0 || tv.tv_usec != 0))
            {
                ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
                if (ready < 0)
                {
                    fprintf(stderr, "select error: %s\n", strerror(errno));
                    return std::make_pair(EXIT_FAILURE, std::vector<std::pair<std::string, double>>());
                }
            }
        }
    }
    std::string ip_result = "";
    for (auto el = ip_addresses_response.begin(); el != ip_addresses_response.end(); ++el)
    {
        ip_result += *el + " ";
    }
    result.push_back(std::make_pair(ip_result, (double)how_many_responses_per_ttl));
    result.push_back(std::make_pair(std::to_string(ip_addresses_response.size()), RTT));
    return std::make_pair(EXIT_SUCCESS, result);
}
