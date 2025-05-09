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
#ifndef _LIBRCOM_ISYSTEM_H
#define _LIBRCOM_ISYSTEM_H

#include "rcom/IAddress.h"

namespace rcom {

        enum WaitStatus {
                kWaitOK = 1,
                kWaitTimeout = 0,
                kWaitError = -1,
        };

        class ISystem {
        public:
                static const int kInvalidSocket = -1;
                
                virtual ~ISystem() = default;

                // Clock
                virtual double time() = 0;
                virtual void sleep(double seconds) = 0;

                virtual std::string local_ip() = 0;
                virtual void set_local_ip(const std::string& ip) = 0;
                
                // UDP & TCP sockets
                virtual int tcp_server_socket(const IAddress& address) = 0;
                virtual int accept(int sockfd) = 0;                
                virtual int tcp_client_socket(const IAddress& address) = 0;
                virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) = 0;
                virtual ssize_t send(int sockfd, const void *buf, size_t len) = 0;
                virtual int socket_close(int fd) = 0;                                
                virtual int getaddress(int sockfd, IAddress& address) = 0;                
                virtual int setsockopt(int sockfd, int level, int optname,
                                       const void *optval, int optlen) = 0;
                                
                // Random
                virtual ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) = 0;
        
                // Poll
                virtual WaitStatus wait(int sockfd, int timeout) = 0;
        };
}
#endif // _LIBRCOM_ISYSTEM_H
