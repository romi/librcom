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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "ConsoleLogger.h"
#include "BaseSocket.h"

namespace rcom {

        BaseSocket::BaseSocket(std::shared_ptr<rpp::ILinux>& linux)
                : linux_(std::move(linux)), sockfd_(kInvalidSocket)
        {
        }

        BaseSocket::BaseSocket(std::shared_ptr<rpp::ILinux>& linux, int sockfd)
                : linux_(std::move(linux)), sockfd_(sockfd)
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
                        ssize_t n = linux_->send(sockfd_, buffer + sent,
                                                length - sent, MSG_NOSIGNAL);
                        
                        if (n < 0) {
                                log_error("socket_send: send failed: %s", strerror(errno));
                                success = false;
                                break;
                        } else if (n == 0) {
                                log_error("socket_send: send() returned zero");
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
                        
                        ssize_t n = linux_->recv(sockfd_, buffer + received, requested, 0);
                        
                        if (n < -1) {
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
                bool success = false;
                socklen_t addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in addr = address.get_sockaddr();
                int sockfd = linux_->socket(AF_INET, SOCK_STREAM, 0);
                
                if (sockfd != kInvalidSocket) {
                        
                        int ret = linux_->connect(sockfd, (struct sockaddr *) &addr, addrlen);
                        
                        if (ret == 0) {
                                sockfd_ = sockfd;
                                success = true;

                        } else {
                                log_error("Socket::connect: failed to bind the socket");
                                linux_->close(sockfd);
                        }
                } else {
                        log_error("Socket::connect: failed to create the socket");
                }
        
                return success;
        }
        
        bool BaseSocket::listen(IAddress& address)
        {
                bool success = false;
                int ret;
                struct sockaddr_in addr = address.get_sockaddr();
                uint32_t socklen = sizeof(struct sockaddr_in);

                sockfd_ = linux_->socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd_ != kInvalidSocket) {
                        
                        ret = linux_->bind(sockfd_, (struct sockaddr *) &addr, socklen);
                        if (ret == 0) {
        
                                ret = linux_->listen(sockfd_, 10);
                                if (ret == 0) {
                                        success = true;
                                        
                                } else {
                                        log_error("ServerSocket::open: listen failed: %s",
                                              strerror(errno));
                                }
                                
                        } else {
                                log_error("ServerSocket::open: bind failed: %s",
                                      strerror(errno));
                        }

                } else {
                        log_error("ServerSocket::open: socket failed: %s", strerror(errno));
                }
        
                return success;
        }

        int BaseSocket::accept(double timeout_in_seconds)
        {
                uint32_t addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in addr{};
                int clientfd = kInvalidSocket;
                
                // The code waits for incoming connections for one
                // second. 
                WaitStatus wait_status = wait(timeout_in_seconds);
                
                if (wait_status == kWaitOK) {
                        clientfd = linux_->accept(sockfd_, (struct sockaddr*) &addr, &addrlen);
                        if (clientfd < 0) {
                                // Server socket is probably being closed
                                // FIXME: is this true?
                                log_error("server_socket_accept: accept failed: %s",
                                      strerror(errno)); 
                        }
                }

                return clientfd;
        }

        bool BaseSocket::is_connected() const
        {
                return (sockfd_ != kInvalidSocket);
        }

        bool BaseSocket::is_endpoint_connected() const {
            char buffer[32];
            bool connected = true;
            // if recv returns zero, that means the connection has been closed:
            if (recv(sockfd_, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0)
                connected = false;
//            log_debug("BaseSocket::is_endpoint_connected() socketfd = %d = %s", sockfd_, (connected ? "true" : "false"));
            return connected;
        }

        void BaseSocket::get_address(IAddress& address)
        {
                struct sockaddr_in local_addr{};
                uint32_t socklen = sizeof(local_addr);
                memset((char *) &local_addr, 0, socklen);
                
                linux_->getsockname(sockfd_, (struct sockaddr*) &local_addr, &socklen);
                
                address.set(inet_ntoa(local_addr.sin_addr),
                            ntohs(local_addr.sin_port));
        }

        WaitStatus BaseSocket::wait(double timeout)
        {
                WaitStatus ret = kWaitError;
                if (timeout >= 0.0)
                        ret = do_wait(timeout);
                return ret; 
        }

        WaitStatus BaseSocket::do_wait(double timeout)
        {
                WaitStatus retval = kWaitError;
                int timeout_ms = (int) (timeout * 1000.0);
                
                struct pollfd fds[1];
                fds[0].fd = sockfd_;
                fds[0].events = POLLIN;
                
                int pollrc = linux_->poll(fds, 1, timeout_ms);
                if (pollrc < 0) {
                        log_error("do_wait: poll error %d", errno);
                        
                } else if (pollrc > 0) {
                        if (fds[0].revents & POLLIN) {
                                retval = kWaitOK;
                        }
                } else {
                        retval = kWaitTimeout;
                }
                return retval;
        }
        
        void BaseSocket::set_nodelay(int value)
        {
                linux_->setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                                 (char *) &value, sizeof(int));
        }        

        void BaseSocket::close()
        {
                ssize_t n;
                char buf[512];
                if (sockfd_ != kInvalidSocket) {
                        linux_->shutdown(sockfd_, SHUT_RDWR);
                        while (true) {
                                n = linux_->recv(sockfd_, buf, 512, 0);
                                if (n <= 0)
                                        break;
                        }
                        linux_->close(sockfd_);
                        sockfd_ = kInvalidSocket;
                }
        }
}
