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
#include "rcom/WebSocketServerFactory.h"
#include "rcom/RcomMessageHandler.h"

namespace rcom {

        std::unique_ptr<IRPCServer> RcomServer::create(const std::string& topic,
                                                       IRPCHandler &handler)
        {
                auto webserver_socket_factory = WebSocketServerFactory::create();
                std::shared_ptr<ISocketFactory> socket_factory
                        = std::make_shared<SocketFactory>();
                std::shared_ptr<IMessageListener> listener
                        = std::make_shared<RcomMessageHandler>(handler);
                std::unique_ptr<IMessageHub> hub
                        = std::make_unique<MessageHub>(topic, listener,
                                                       socket_factory,
                                                       webserver_socket_factory);
                return std::make_unique<RcomServer>(hub);
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
