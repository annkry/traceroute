#include "icmpchecksum.h"
#include <cassert>

uint16_t compute_icmp_checksum(const void *buff, int length)
{
    uint32_t sum;
    const uint16_t *ptr = static_cast<const uint16_t *>(buff);
    assert(length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return static_cast<uint16_t>(~(sum + (sum >> 16)));
}
