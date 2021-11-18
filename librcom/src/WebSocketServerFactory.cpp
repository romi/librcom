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
#include <memory>
#include <Linux.h>
#include "SocketFactory.h"
#include "WebSocketServerFactory.h"

#include "ServerSocket.h"
#include "Address.h"
#include "Response.h"
#include "WebSocketServer.h"

namespace rcom {
        
    WebSocketServerFactory::WebSocketServerFactory( const std::shared_ptr<ISocketFactory>& socket_factory) : socket_factory_(socket_factory)
    {
    }

    std::unique_ptr<IWebSocketServer>
    WebSocketServerFactory::new_web_socket_server(const std::shared_ptr<IMessageListener> &listener, uint16_t port)
    {
        Address address(port);
        std::unique_ptr<rpp::ILinux> linux
                = std::make_unique<rpp::Linux>();

        std::unique_ptr<IServerSocket> server_socket
                = std::make_unique<ServerSocket>(linux, address);


        return std::make_unique<WebSocketServer>(server_socket,
                                                    socket_factory_,
                                                    listener);
    }

    std::shared_ptr<IWebSocketServerFactory> WebSocketServerFactory::create() {

        std::shared_ptr<rcom::ISocketFactory> socket_factory = std::make_shared<rcom::SocketFactory>();
        return std::make_shared<rcom::WebSocketServerFactory>(socket_factory);
    }
}
