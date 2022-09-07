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

#include <stdexcept>
#include <string.h>
#include "rcom/RcomServer.h"
#include "rcom/MessageHub.h"
#include "rcom/RcomMessageHandler.h"
#include "rcom/MessageHub.h"
#include "rcom/ConsoleLog.h"
#include "rcom/Log.h"
#include "rcom/Address.h"
#include "rcom/ServerSocket.h"
#include "rcom/WebSocketServer.h"

namespace rcom {

        std::unique_ptr<IRPCServer> RcomServer::create(const std::string& topic,
                                                       IRPCHandler &handler,
                                                       const std::shared_ptr<ILog>& log)
        {
                Address address(0);
                std::shared_ptr<ILinux> linux = std::make_shared<Linux>();
                std::shared_ptr<ISocketFactory> socket_factory
                        = std::make_shared<SocketFactory>(linux, log);
                std::shared_ptr<IMessageListener> listener
                        = std::make_shared<RcomMessageHandler>(handler);
                std::unique_ptr<IServerSocket> server_socket
                        = std::make_unique<ServerSocket>(linux, log, address);
                std::unique_ptr<IWebSocketServer> ws_server
                        = std::make_unique<WebSocketServer>(server_socket, socket_factory,
                                                            listener, log);
                std::unique_ptr<IMessageHub> hub
                        = std::make_unique<MessageHub>(topic, ws_server, socket_factory,
                                                       linux, log);
                return std::make_unique<RcomServer>(hub);
        }

        std::unique_ptr<IRPCServer> RcomServer::create(const std::string& topic,
                                                       IRPCHandler &handler)
        {
                std::shared_ptr<ILog> log = std::make_shared<ConsoleLog>();
                return create(topic, handler, log);
        }
        
        RcomServer::RcomServer(std::unique_ptr<IMessageHub>& hub)
                : hub_(std::move(hub))
        {
        }

        void RcomServer::handle_events()
        {
                hub_->handle_events();
        }
}
