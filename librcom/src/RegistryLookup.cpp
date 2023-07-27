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
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>

#include "rcom/RegistryLookup.h"

namespace rcom {

        RegistryLookup::RegistryLookup(uint16_t port)
                : port_(port),
                  socket_(-1),
                  address_()
        {
        }

        void RegistryLookup::create_socket()
        {
                socket_ = socket(AF_INET, SOCK_DGRAM, 0);
                if (socket_ < 0) {
                        throw std::runtime_error("Failed to create socket.");
                }

                // Enable broadcasting
                int broadcastEnable = 1;
                if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST,
                               &broadcastEnable, sizeof(broadcastEnable)) < 0) {
                        close(socket_);
                        throw std::runtime_error("Failed to enable broadcast.");
                }

                // Set timeout to 15 seconds
                struct timeval tv;
                tv.tv_sec = 15;
                tv.tv_usec = 0;
                if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                        close(socket_);
                        throw std::runtime_error("Failed to set the timeout.");
                }
        }
        
        void RegistryLookup::send_message()
        {
                // Set up the server address for broadcasting
                struct sockaddr_in serverAddr;
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_port = htons(port_);
                serverAddr.sin_addr.s_addr = INADDR_BROADCAST;

                std::string broadcastMessage = "who-has-the-registry-ip";
        
                // Send the broadcast message
                ssize_t bytesSent = sendto(socket_, broadcastMessage.c_str(),
                                           broadcastMessage.length(), 0,
                                           (struct sockaddr*) &serverAddr,
                                           sizeof(serverAddr));
                if (bytesSent < 0) {
                        close(socket_);
                        throw std::runtime_error("Failed to send broadcast message.");
                }
        }

        void RegistryLookup::get_response()
        {
                char buffer[1024];
                struct sockaddr_in senderAddr;
                socklen_t senderAddrLen = sizeof(senderAddr);
                
                ssize_t bytesRead = recvfrom(socket_, buffer,
                                             sizeof(buffer) - 1, 0,
                                             (struct sockaddr*) &senderAddr,
                                             &senderAddrLen);
                if (bytesRead > 0) {
                        buffer[bytesRead] = '\0';
                        address_ = buffer;
                }
        }

        std::string RegistryLookup::lookup()
        {
                create_socket();

                for (int i = 0; i < 8; i++) {
                        send_message();
                        get_response();
                        if (!address_.empty())
                                break;
                }
                close(socket_);
                
                if (address_.empty()) {
                        throw std::runtime_error("Failed to receive data.");
                }

                return address_;
        }
}
