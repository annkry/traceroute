#include <sys/time.h>

int send(int ID, int sockfd, int ttl, int nr_package, const char *destination, timeval& send_time);