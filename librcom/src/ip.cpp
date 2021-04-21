#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <r.h>

namespace rcom {

// https://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
        bool get_ip(std::string& ip)
        {
                bool success = false;
                struct ifaddrs *ifaddr;
                struct ifaddrs *ifa;
        
                if (getifaddrs(&ifaddr) == -1) {
                        perror("getifaddrs");
                        return false;
                }

                for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                
                        if (ifa->ifa_addr == NULL)
                                continue;  

                        if ((strncmp(ifa->ifa_name, "w", 1) == 0)
                            && (ifa->ifa_addr->sa_family == AF_INET)) {
                        
                                char host[NI_MAXHOST];
                                int s = getnameinfo(ifa->ifa_addr,
                                                    sizeof(struct sockaddr_in),
                                                    host, NI_MAXHOST,
                                                    NULL, 0,
                                                    NI_NUMERICHOST);
                        
                                if (s == 0) {
                                        ip = host;
                                        success = true;
                                        break;

                                } else {
                                        r_warn("getnameinfo() failed for interface %s: "
                                               "%s\n", ifa->ifa_name, gai_strerror(s));
                                }
                        }
                }

                freeifaddrs(ifaddr);
                return success;
        }
}
