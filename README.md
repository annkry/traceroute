# traceroute
The program will send ICMP echo request packets with increasing TTL values (similar to the traceroute -I command). For each TTL value ∈ [1, 30], the program will perform the following operations:
 - sends 3 ICMP echo request packets with a set TTL (one after another, without waiting for a response),
 - receives responses to these packets from the socket, but will not wait for them for longer than one second. If all 3 responses arrive faster than after one second, proceeds to the next point immediately. Any responses to packets from previous iterations (i.e. iterations in which packets were sent with smaller TTL values) will be ignored,
 - displays the IP address of the router from which the messages will be received and the average response time in milliseconds. If there is no response from any router, displays `*`. If there is a response from more than one router, displays all corresponding ones. If three responses are not received within the specified time, displays `???`.

To compile: make  
To run: sudo ./traceroute <ip_address>
