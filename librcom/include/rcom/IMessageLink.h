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
#ifndef _LIBRCOM_I_MESSAGE_LINK_H_
#define _LIBRCOM_I_MESSAGE_LINK_H_

#include <string>
#include "rcom/MemBuffer.h"
#include "rcom/IWebSocket.h"

namespace rcom {

        class IMessageLink
        {
        public:                
                virtual ~IMessageLink() = default;

                virtual std::string& get_topic() = 0;
                
                virtual bool recv(MemBuffer& message, double timeout = 0.0) = 0;
                virtual bool send(MemBuffer& message,
                                  MessageType type = kTextMessage) = 0;

                virtual RecvStatus recv_status() = 0;
                virtual bool is_connected() = 0;
        };
}

#endif // _LIBRCOM_I_MESSAGE_LINK_H_
