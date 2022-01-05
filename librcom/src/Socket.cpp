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
#include <netinet/tcp.h>
#include <stdexcept>
#include "ConsoleLogger.h"
#include "Socket.h"

namespace rcom {

        Socket::Socket(std::shared_ptr<rpp::ILinux>& linux, int sockfd)
                : socket_(linux, sockfd)
        {
        }

        Socket::Socket(std::shared_ptr<rpp::ILinux>& linux, IAddress& address)
                : socket_(linux)
        {
                if (!socket_.connect(address)) {
                        std::string s;
                        log_error("Socket::Socket: Failed to connect to address %s",
                              address.tostring(s).c_str());
                        throw std::runtime_error("Socket: Failed to connect");
                }
        }
        
        bool Socket::is_connected()
        {
                return socket_.is_connected();
        }

        bool Socket::is_endpoint_connected() {
            return socket_.is_endpoint_connected();
        }

        void Socket::close()
        {
                socket_.close();
        }
        
        bool Socket::send(rpp::MemBuffer& buffer)
        {
                const std::vector<uint8_t>& data = buffer.data();;
                return send(&data[0], buffer.size());
        }

        bool Socket::send(const uint8_t *buffer, size_t length)
        {
                return socket_.write(buffer, length);
        }
        
        bool Socket::read(uint8_t *buffer, size_t length)
        {
                return socket_.read(buffer, length);
        }
        
        WaitStatus Socket::wait(double timeout)
        {
                return socket_.wait(timeout);
        }

        void Socket::turn_buffering_off()
        {
                socket_.set_nodelay(1);
        }
                
        void Socket::turn_buffering_on()
        {
                socket_.set_nodelay(0);
        }
}
