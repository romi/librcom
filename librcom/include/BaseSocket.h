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
#ifndef _LIBRCOM_BASE_SOCKET_H_
#define _LIBRCOM_BASE_SOCKET_H_

#include <string>
#include <memory>
#include <ILinux.h>
#include "IAddress.h"
#include "ISocket.h"

namespace rcom {

        class BaseSocket {
        protected:
                std::shared_ptr<rcom::ILinux> linux_;
                int sockfd_;
                
                WaitStatus do_wait(double timeout);
                
        public:

                BaseSocket(std::shared_ptr<rcom::ILinux>& linux);
                BaseSocket(std::shared_ptr<rcom::ILinux>& linux, int sockfd);
                virtual ~BaseSocket();
                        
                bool listen(IAddress& address);
                int accept(double timeout_in_seconds);
                bool connect(IAddress& address);
                bool write(const uint8_t *buffer, size_t length);
                bool read(uint8_t *buffer, size_t length);                
                WaitStatus wait(double timeout);
                void close();
                
                bool is_connected() const;
                bool is_endpoint_connected() const;
                void get_address(IAddress& address);
                void set_nodelay(int value);

                ILinux& get_linux();
        };
}

#endif // _LIBRCOM_BASE_SOCKET_H_

