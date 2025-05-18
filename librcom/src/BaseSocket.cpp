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

#include <string.h>
#include <sys/socket.h> // FIXME: for MSG_PEEK and MSG_DONTWAIT
#include <netinet/in.h> // FIXME: for IPPROTO_TCP
#include <netinet/tcp.h> // FIXME: for TCP_NODELAY
#include "rcom/Log.h"
#include "rcom/BaseSocket.h"

namespace rcom {

        BaseSocket::BaseSocket(ILog& log, ISystem& system)
                : BaseSocket(log, system, kInvalidSocket)
        {
        }

        BaseSocket::BaseSocket(ILog& log, ISystem& system, int sockfd)
                : log_(log), system_(system), sockfd_(sockfd)
        {
        }

        BaseSocket::~BaseSocket()
        {
                close();
        }

        bool BaseSocket::write(const uint8_t *buffer, size_t length)
        {
                bool success = true;
                size_t sent = 0;
        
                while (sent < length) {
                        // Using MSG_NOSIGNAL to prevent SIGPIPE signals in
                        // case the client closes the connection before all
                        // the data is sent.
                        ssize_t n = system_.send(sockfd_, buffer + sent, length - sent);
                        
                        if (n < 0) {
                                log_err(log_, "socket_send: send failed: %s",
                                        strerror(errno));
                                success = false;
                                break;
                        } else if (n == 0) {
                                log_err(log_, "socket_send: send() returned zero");
                                success = false;
                                break;
                        } else {                        
                                sent += (size_t) n;
                        }
                }
        
                return success;
        }
        
        bool BaseSocket::read(uint8_t *buffer, size_t length)
        {
                bool success = true;
                size_t received = 0;

                while (received < length) {

                        size_t requested = length - received;
                        
                        ssize_t n = system_.recv(sockfd_, buffer + received, requested, 0);
                        
                        if (n < 0) {
                                // Error
                                if (errno != EAGAIN) {
                                        success = false;
                                        break;
                                }
                        } else if (n == 0) {
                                // End of stream / stream closed
                                success = false;
                                break;
                        } else { // n > 0
                                received += (size_t) n;
                        }
                }
                
                return success;
        }

        bool BaseSocket::connect(IAddress& address)
        {
                bool success = true;
                sockfd_ = system_.tcp_client_socket(address);
                if (sockfd_ == kInvalidSocket) {
                        log_err(log_, "Socket::connect: failed to create the socket");
                        success = false;
                }
                return success;
        }
        
        bool BaseSocket::listen(IAddress& address)
        {
                bool success = true;
                sockfd_ = system_.tcp_server_socket(address);
                if (sockfd_ == kInvalidSocket) {
                        log_err(log_, "ServerSocket::open: socket failed: %s",
                                strerror(errno));
                        success = false;
                }
                return success;
        }

        int BaseSocket::accept(double timeout_in_seconds)
        {
                int clientfd = kInvalidSocket;
                
                // The code waits for incoming connections for one
                // second. 
                WaitStatus wait_status = wait(timeout_in_seconds);
                
                if (wait_status == kWaitOK) {
                        clientfd = system_.accept(sockfd_);
                        if (clientfd < 0) {
                                // Server socket is probably being closed
                                // FIXME: is this true?
                                log_err(log_, "server_socket_accept: accept failed: %s",
                                        strerror(errno)); 
                        }
                }

                return clientfd;
        }

        bool BaseSocket::is_connected() const
        {
                return (sockfd_ != kInvalidSocket);
        }

        bool BaseSocket::is_endpoint_connected() const
        {
                char buffer[32];
                bool connected = true;
                // if recv returns zero, that means the connection has
                // been closed:
                if (system_.recv(sockfd_, buffer, sizeof(buffer),
                                 MSG_PEEK | MSG_DONTWAIT) == 0)
                        connected = false;
                return connected;
        }

        void BaseSocket::get_address(IAddress& address)
        {
                system_.getaddress(sockfd_, address);
        }

        WaitStatus BaseSocket::wait(double timeout)
        {
                WaitStatus ret = kWaitError;
                int timeout_ms = (int) (timeout * 1000.0);
                if (timeout >= 0.0)
                        ret = system_.wait(sockfd_, timeout_ms);
                return ret; 
        }
        
        void BaseSocket::set_nodelay(int value)
        {
                system_.setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                                   (char *) &value, sizeof(int));
        }        

        void BaseSocket::close()
        {
                if (sockfd_ != kInvalidSocket) {
                        system_.socket_close(sockfd_);
                        sockfd_ = kInvalidSocket;
                }
        }

        ISystem& BaseSocket::get_system()
        {
                return system_;
        }
}
