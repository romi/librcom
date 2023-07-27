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
#include <unistd.h>
#include <string.h>
#include <stdexcept>

#include "rcom/ip.h"
#include "rcom/RegistryLookupServer.h"

namespace rcom {

        RegistryLookupServer::RegistryLookupServer(const std::string& address,
                                                   uint16_t port)
                : socket_(-1),
                  thread_(nullptr),
                  address_(address),
                  quit_(false)
        {
                init(port);
                start();
        }

        RegistryLookupServer::~RegistryLookupServer()
        {
                stop();
                close(socket_);
        }

        void RegistryLookupServer::init(uint16_t port)
        {
                // Create a UDP socket
                socket_ = socket(AF_INET, SOCK_DGRAM, 0);
                if (socket_ < 0) {
                        throw std::runtime_error("Failed to create socket.");
                }

                // Enable broadcasting
                int broadcastEnable = 1;
                if (setsockopt(socket_, SOL_SOCKET, SO_BROADCAST,
                               &broadcastEnable, sizeof(broadcastEnable)) < 0) {
                        throw std::runtime_error("Failed to enable broadcast.");
                }

                // Bind the socket to the server address and port
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr.s_addr = INADDR_ANY;
                addr.sin_port = htons(port);

                if (bind(socket_,
                         (struct sockaddr*) &addr,
                         sizeof(addr)) < 0) {
                        throw std::runtime_error("Failed to bind socket to address.");
                }
        }

        void RegistryLookupServer::receiveLookupRequests()
        {
                char buffer[1024];
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);

                while (!quit_) {
                        memset(buffer, 0, sizeof(buffer));
                        ssize_t bytesRead = recvfrom(socket_, buffer,
                                                     sizeof(buffer) - 1, 0,
                                                     (struct sockaddr*) &clientAddr,
                                                     &clientAddrLen);

                        if (bytesRead < 0) {
                                //std::cerr << "Failed to receive data." << std::endl;
                                continue;
                        }

                        sendto(socket_, address_.c_str(), address_.length(), 0,
                               (struct sockaddr*)& clientAddr, clientAddrLen);
                }
        }

        void RegistryLookupServer::start()
        {
                // Run receiveLookupRequests in a separate thread                
                thread_ = std::make_unique<std::thread>(&RegistryLookupServer::receiveLookupRequests,
                                                        this);
        }

        void RegistryLookupServer::stop()
        {
                quit_ = true;
                if (thread_ && thread_->joinable())
                        thread_->join();
        }
};
