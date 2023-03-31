#include <vector>
#include <string>
#include <set>

std::pair<int, std::vector<std::pair<std::string, double>>> receive(int sockfd, std::set<int> ip_id_set, std::set<int> ip_seq_set,  std::vector<std::pair<int, timeval>> time_frame);