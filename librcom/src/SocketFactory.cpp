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
#include "Linux.h"
#include "SocketFactory.h"
#include "Socket.h"
#include "Address.h"
#include "Response.h"
#include "ResponseParser.h"
#include "Request.h"
#include "RequestParser.h"
#include "ServerSideWebSocket.h"
#include "ClientSideWebSocket.h"


namespace rcom {
        
        SocketFactory::SocketFactory()
        = default;

        std::unique_ptr<IWebSocket>
        SocketFactory::new_server_side_websocket(int sockfd)
        {
                Request request;
                RequestParser parser(request);
                std::shared_ptr<rcom::ILinux> linux = std::make_shared<rcom::Linux>();
                std::unique_ptr<ISocket> socket = std::make_unique<Socket>(linux, sockfd);
                return std::make_unique<ServerSideWebSocket>(socket, parser);
        }
        
        std::unique_ptr<IWebSocket>
        SocketFactory::new_client_side_websocket(IAddress& remote_address)
        {
                Response response;
                ResponseParser parser(response);
                std::shared_ptr<rcom::ILinux> linux = std::make_shared<rcom::Linux>();
                std::unique_ptr<ISocket> socket
                        = std::make_unique<Socket>(linux, remote_address);
                return std::make_unique<ClientSideWebSocket>(linux, socket, parser,
                                                             remote_address);
        }
}
