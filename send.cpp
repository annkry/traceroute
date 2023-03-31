#include "send.h"
#include "icmpchecksum.h"
#include <netinet/ip_icmp.h>
#include <iostream>
#include <cerrno>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>

int send(int ID, int sockfd, int ttl, int nr_package, const char *destination, timeval &send_time)
{
    struct timeval send_time_res;
    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = ID + ttl + nr_package;
    header.icmp_hun.ih_idseq.icd_seq = nr_package;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum(
        (u_int16_t *)&header, sizeof(header));
    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    int exit_code = inet_pton(AF_INET, destination, &recipient.sin_addr);
    if (exit_code < 0)
    {
        fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    gettimeofday(&send_time_res, nullptr);
    ssize_t sending = sendto(
        sockfd,
        &header,
        sizeof(header),
        0,
        (struct sockaddr *)&recipient,
        sizeof(recipient));
    if (sending < 0)
    {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    send_time = send_time_res;
    return EXIT_SUCCESS;
}
