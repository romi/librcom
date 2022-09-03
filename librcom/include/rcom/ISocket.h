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
#ifndef _LIBRCOM_I_SOCKET_H_
#define _LIBRCOM_I_SOCKET_H_

#include "rcom/MemBuffer.h"
#include "rcom/IAddress.h"
#include "rcom/ILinux.h"

namespace rcom {

        static const int kInvalidSocket = -1;

        enum WaitStatus {
                kWaitOK = 1,
                kWaitTimeout = 0,
                kWaitError = -1,
        };

        class ISocket
        {
        public:
                
                static const int NoSocket = -1;        

                virtual ~ISocket() = default;
                
                virtual void close() = 0;
                virtual bool is_connected() = 0;
                virtual bool is_endpoint_connected() = 0;

                /* Returns true if the data was sucessfully written
                 * and false in case of error. */
                virtual bool send(rcom::MemBuffer& buffer) = 0;
                virtual bool send(const uint8_t *buffer, size_t length) = 0;

                /* This function blocks until all data has been
                 * read. It retuns true when the requested number of
                 * bytes have been read. In case the socket was shut
                 * down or in case an error occurs before the
                 * requested number of bytes was read , the method
                 * returns false.
                 */
                virtual bool read(uint8_t *buffer, size_t length) = 0;
                
                virtual WaitStatus wait(double timeout_in_seconds) = 0;

                virtual void turn_buffering_off() = 0;
                virtual void turn_buffering_on() = 0;

                virtual ILinux& get_linux() = 0;
        };
}

#endif // _LIBRCOM_I_SOCKET_H_
