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
#ifndef _LIBRCOM_SOCKET_H_
#define _LIBRCOM_SOCKET_H_

#include "rcom/ILinux.h"
#include "rcom/ISocket.h"
#include "rcom/BaseSocket.h"

namespace rcom {
        
        class Socket : public ISocket 
        {
        protected:
                BaseSocket socket_;

        public:

                Socket(const std::shared_ptr<ILinux>& linux,
                       const std::shared_ptr<ILog>& log,
                       int sockfd);
                Socket(const std::shared_ptr<ILinux>& linux,
                       const std::shared_ptr<ILog>& log,
                       IAddress& address);
                ~Socket() override = default;
                
                void close() override;
                bool is_connected() override;
                bool is_endpoint_connected() override;
                
                bool send(MemBuffer& buffer) override;
                bool send(const uint8_t *buffer, size_t length) override;
                bool read(uint8_t *buffer, size_t length) override;

                WaitStatus wait(double timeout_in_seconds) override;

                void turn_buffering_off() override;
                void turn_buffering_on() override;
                
                ILinux& get_linux() override;
        };
}

#endif // _LIBRCOM_SOCKET_H_
