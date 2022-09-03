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
#ifndef _LIBRCOM_I_WEB_SOCKET_SERVER_FACTORY_H_
#define _LIBRCOM_I_WEB_SOCKET_SERVER_FACTORY_H_

#include <memory>
#include "rcom/IAddress.h"
#include "rcom/ISocket.h"
#include "rcom/IServerSocket.h"
#include "rcom/IWebSocket.h"
#include "rcom/IMessageListener.h"
#include "rcom/IWebSocketServer.h"

namespace rcom {
        
        class IWebSocketServerFactory
        {
        public:
                virtual ~IWebSocketServerFactory() = default;
                virtual std::unique_ptr<IWebSocketServer>
                        new_web_socket_server(const std::shared_ptr<IMessageListener> &listener, uint16_t port) = 0;
        };
}

#endif
