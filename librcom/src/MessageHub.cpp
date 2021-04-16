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
#include <r.h>
#include <algorithm>
#include "MessageHub.h"
#include "Address.h"
#include "ClientSideWebSocket.h"
#include "WebSocketServer.h"
#include "RegistryServer.h"
#include "RegistryProxy.h"
#include "ServerSocket.h"
#include "Clock.h"
#include "util.h"

namespace rcom {
        
        MessageHub::MessageHub(const std::string& topic, IMessageListener& listener)
                : linux_(),
                  clock_(),
                  factory_(linux_, clock_),
                  server_(),
                  topic_(topic)
        {
                if (!is_valid_topic(topic)) {
                        r_err("MessageHub: Invalid topic: %s", topic.c_str());
                        throw std::runtime_error("MessageHub: Invalid topic");
                }

                Address address(0);                
                std::unique_ptr<IServerSocket> server_socket
                        = std::make_unique<ServerSocket>(linux_, address);
                
                server_ = std::make_unique<WebSocketServer>(server_socket, factory_, listener);
                
                if (!register_topic()) {
                        r_err("MessageHub: Registration failed: topic '%s'", topic.c_str());
                        throw std::runtime_error("MessageHub: Registration failed");
                }
        }
        
        MessageHub::MessageHub(const std::string& topic)
                : MessageHub(topic, *this)
        {
        }
        
        std::string& MessageHub::topic()
        {
                return topic_;
        }
        
        MessageHub::~MessageHub()
        {
        }

        bool MessageHub::register_topic()
        {
                Address registry_address;                
                RegistryServer::get_address(registry_address);

                std::unique_ptr<IWebSocket> registry_socket
                        = factory_.new_client_side_websocket(registry_address);

                Clock clock;
                RegistryProxy registry(registry_socket, clock);
                
                Address my_address;
                server_->get_address(my_address);

                return registry.set(topic_, my_address);
        }

        void MessageHub::handle_events()
        {
                server_->handle_events();
        }

        void MessageHub::broadcast(rpp::MemBuffer& message,
                                   IWebSocket* exclude,
                                   MessageType type)
        {
                server_->broadcast(message, exclude, type);
        }

        void MessageHub::onmessage(IWebSocket& link, rpp::MemBuffer& message)
        {
                (void) link;
                (void) message;
                r_warn("MessageHub::onmessage: Received unhandled message");
        }
}
