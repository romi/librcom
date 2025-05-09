/*
  rcom

  Copyright (C) 2019 Sony Computer Science Laboratories
  Author(s) Peter Hanappe

  rcom is light-weight libary for inter-node communication.

  rcom is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.

 */
#ifndef _LIBRCOM_LINUX_H
#define _LIBRCOM_LINUX_H

#include <time.h>
#include <sys/socket.h>
#include <poll.h>
#include <ifaddrs.h>
#include "rcom/ILog.h"
#include "rcom/ISystem.h"

namespace  rcom {

        class Linux : public ISystem {
        protected:
                ILog& log_;
                std::string ip_;
 
        public:
                
                Linux(ILog& log);
                virtual ~Linux() = default;
                
                int tcp_server_socket(const IAddress& address) override;
                int tcp_client_socket(const IAddress& address) override;
                int accept(int sockfd) override;
                int socket_close(int fd) override;                
                ssize_t recv(int sockfd, void *buf, size_t len, int flags) override;
                ssize_t send(int sockfd, const void *buf, size_t len) override;
                int getaddress(int sockfd, IAddress& address) override;
                int setsockopt(int sockfd, int level, int optname,
                               const void *optval, int optlen) override;
                double time() override;
                void sleep(double seconds) override;
                ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) override;
                WaitStatus wait(int sockfd, int timeout) override;
                std::string local_ip() override;
                void set_local_ip(const std::string& ip) override;
                
        protected:
                void address_to_sockaddr(const IAddress& address, struct sockaddr_in& addr);
                void sockaddr_to_address(IAddress& address, const struct sockaddr_in& addr);
                int bind(int sockfd, const IAddress& address);

                // POSIX
                int socket();
                int connect(int sockfd, const IAddress& address);
                int shutdown(int sockfd);
                int listen(int sockfd, int backlog);
                int close(int fd);                
                int poll(struct pollfd *fds, nfds_t nfds, int timeout);
                int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
                int clock_gettime(clockid_t clockid, struct timespec *tp);
                int clock_nanosleep(clockid_t clock_id, int flags,
                                    const struct timespec *request,
                                    struct timespec *remain);
                int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
                void scan_interfaces(std::string& ip, struct ifaddrs *ifaddr);
                bool match_interface(struct ifaddrs *ifa);
                void get_interface_ip(std::string& ip, struct ifaddrs *ifa);
        };
}

#endif // _LIBRCOM_LINUX_H
