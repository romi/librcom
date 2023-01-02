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
#include "rcom/Log.h"
#include "rcom/MessageHub.h"
#include "rcom/WebSocketServer.h"
#include "rcom/util.h"
#include "rcom/DummyMessageListener.h"
#include "rcom/ConsoleLog.h"
#include "rcom/Address.h"
#include "rcom/ServerSocket.h"
#include "rcom/RegistryServer.h"
#include "rcom/RegistryProxy.h"

namespace rcom {
        
        std::unique_ptr<IMessageHub>
        MessageHub::create(const std::string& topic,
                           const std::shared_ptr<IMessageListener>& listener,
                           const std::shared_ptr<ILog>& log,
                           uint16_t port,
                           bool standalone)
        {
                Address address(port);
                auto linux = std::make_shared<Linux>();
                auto socket_factory = std::make_shared<SocketFactory>(linux, log);
                std::unique_ptr<IServerSocket> server_socket
                        = std::make_unique<ServerSocket>(linux, log, address);
                std::unique_ptr<IWebSocketServer> ws_server
                        = std::make_unique<WebSocketServer>(server_socket, socket_factory,
                                                            listener, log);
                auto hub = std::make_unique<MessageHub>(topic, ws_server, socket_factory,
                                                        linux, log);
                if (!standalone) {
                        hub->register_topic();
                }
                return hub;
        }
 
        std::unique_ptr<IMessageHub>
        MessageHub::create(const std::string& topic,
                           const std::shared_ptr<IMessageListener>& listener,
                           const std::shared_ptr<ILog>& log)
        {
                return create(topic, listener, log, 0, false);
        }
                
        std::unique_ptr<IMessageHub>
        MessageHub::create(const std::string& topic,
                           const std::shared_ptr<IMessageListener>& listener)
        {
                std::shared_ptr<ILog> log = std::make_shared<ConsoleLog>();
                return create(topic, listener, log);
        }
                
        std::unique_ptr<IMessageHub>
        MessageHub::create(const std::string& topic, const std::shared_ptr<ILog>& log)
        {
                std::shared_ptr<IMessageListener> listener
                        = std::make_shared<DummyMessageListener>();
                return create(topic, listener, log);
        }
                
        std::unique_ptr<IMessageHub>
        MessageHub::create(const std::string& topic)
        {
                std::shared_ptr<ILog> log = std::make_shared<ConsoleLog>();
                return create(topic, log);
        }

        MessageHub::MessageHub(const std::string &topic,
                               std::unique_ptr<IWebSocketServer>& server_socket,
                               const std::shared_ptr<ISocketFactory>& socket_factory,
                               const std::shared_ptr<ILinux>& linux,
                               const std::shared_ptr<ILog>& log)
                : server_(std::move(server_socket)),
                  socket_factory_(socket_factory),
                  topic_(topic),
                  linux_(linux),
                  log_(log)
        {
                if (nullptr == server_) {
                        log_err(log_, "MessageHub: Invalid server socket");
                        throw std::invalid_argument("MessageHub: Invalid server socket");
                }
                if (nullptr == socket_factory_) {
                        log_err(log_, "MessageHub: Invalid socket factory");
                        throw std::invalid_argument("MessageHub: Invalid socket factory");
                }
                if (!is_valid_topic(topic)) {
                        log_err(log_, "MessageHub: Invalid topic: %s", topic.c_str());
                        throw std::invalid_argument("MessageHub: Invalid topic");
                }
                // if (!register_topic()) {
                //         log_err(log_, "MessageHub: Registration failed: topic '%s'", topic.c_str());
                //         throw std::runtime_error("MessageHub: Registration failed");
                // }
        }

        std::string& MessageHub::topic()
        {
                return topic_;
        }

        void MessageHub::register_topic()
        {
                Address registry_address;
                RegistryServer::get_address(registry_address);

                std::unique_ptr<IWebSocket> registry_socket
                        = socket_factory_->new_client_side_websocket(registry_address);

                RegistryProxy registry(registry_socket, linux_, log_);

                Address my_address;
                server_->get_address(my_address);

                registry.set(topic_, my_address);
        }

        void MessageHub::handle_events()
        {
                server_->handle_events();
        }

        void MessageHub::broadcast(MemBuffer &message,
                                   MessageType type,
                                   IWebSocket *exclude)
        {
                server_->broadcast(message, type, exclude);
        }

        size_t MessageHub::count_links()
        {
                return server_->count_links();
        }
}
