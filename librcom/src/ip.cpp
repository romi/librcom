#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include "rcom/ip.h"

namespace rcom {

        std::string ip_;
        
        void set_local_ip(const std::string& ip)
        {
                ip_ = ip;
        }
        
        void get_interface_ip(std::string& ip, struct ifaddrs *ifa)
        {
                char host[NI_MAXHOST];
                int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                                    host, NI_MAXHOST,
                                    NULL, 0,
                                    NI_NUMERICHOST);
                        
                if (s == 0) {
                        ip = host;
                } else {
                        throw std::runtime_error("getnameinfo() failed");
                }
        }

        bool match_interface(struct ifaddrs *ifa)
        {
                return ((strncmp(ifa->ifa_name, "w", 1) == 0)
                        && (ifa->ifa_addr->sa_family == AF_INET));
        }
        
        void scan_interfaces(std::string& ip, struct ifaddrs *ifaddr)
        {
                for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                        if ((ifa->ifa_addr != NULL) && match_interface(ifa)) {
                                get_interface_ip(ip, ifa);
                        }
                }
        }
        
        // https://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
        std::string get_local_ip()
        {
                std::string ip = "127.0.0.1"; // By default
                struct ifaddrs *ifaddr;

                if (!ip_.empty()) {
                        return ip_;
                        
                } else  if (getifaddrs(&ifaddr) == 0) {
                        scan_interfaces(ip, ifaddr);
                        freeifaddrs(ifaddr);
                        
                } else {
                        throw std::runtime_error("getifaddrs() failed");
                }

                return ip;
        }
}
