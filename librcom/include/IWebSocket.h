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
#ifndef _LIBRCOM_I_WEBSOCKET_H_
#define _LIBRCOM_I_WEBSOCKET_H_

#include <MemBuffer.h>
#include "IAddress.h"
#include "WebSocketConstants.h"

namespace rcom {
                
        enum MessageType {
                kTextMessage,
                kBinaryMessage
        };
                
        enum RecvStatus {
                kRecvError = -3,
                kRecvClosed = -2,
                kRecvTimeOut = -1,
                kRecvText = 1,
                kRecvBinary = 2
        };

        class IWebSocket
        {
        protected:

        public:
                
                virtual ~IWebSocket() = default;

                virtual RecvStatus recv(rpp::MemBuffer& message, double timeout = 0.0) = 0;
                virtual bool send(rpp::MemBuffer& message, MessageType type) = 0;
                virtual void close(CloseCode code) = 0;
                virtual bool is_connected() = 0;
        };
}

#endif // _LIBRCOM_I_WEBSOCKET_H_
