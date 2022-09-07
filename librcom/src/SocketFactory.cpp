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
#include "rcom/Linux.h"
#include "rcom/SocketFactory.h"
#include "rcom/Socket.h"
#include "rcom/Address.h"
#include "rcom/Response.h"
#include "rcom/ResponseParser.h"
#include "rcom/Request.h"
#include "rcom/RequestParser.h"
#include "rcom/ServerSideWebSocket.h"
#include "rcom/ClientSideWebSocket.h"

namespace rcom {
        
        SocketFactory::SocketFactory(const std::shared_ptr<ILinux>& linux,
                                     const std::shared_ptr<ILog>& log)
                : linux_(linux),
                  log_(log)
        {
        }

        std::unique_ptr<IWebSocket>
        SocketFactory::new_server_side_websocket(int sockfd)
        {
                Request request;
                RequestParser parser(request);
                std::unique_ptr<ISocket> socket
                        = std::make_unique<Socket>(linux_, log_, sockfd);
                return std::make_unique<ServerSideWebSocket>(socket, parser,
                                                             linux_, log_);
        }
        
        std::unique_ptr<IWebSocket>
        SocketFactory::new_client_side_websocket(IAddress& remote_address)
        {
                Response response;
                ResponseParser parser(response);
                std::unique_ptr<ISocket> socket
                        = std::make_unique<Socket>(linux_, log_, remote_address);
                return std::make_unique<ClientSideWebSocket>(socket, parser,
                                                             remote_address,
                                                             linux_, log_);
        }
}
