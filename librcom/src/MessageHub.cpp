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
#include "rcom/Log.h"
#include "rcom/MessageHub.h"
#include "rcom/WebSocketServer.h"
#include "rcom/util.h"
#include "rcom/Address.h"
#include "rcom/ServerSocket.h"
#include "rcom/RegistryServer.h"
#include "rcom/RegistryProxy.h"

namespace rcom {
        
        std::unique_ptr<IMessageHub> MessageHub::create(const std::string& topic,
                                                        const std::string& type,
                                                        IMessageListener& listener,
                                                        ILog& log, ISystem& system,
                                                        uint16_t port, bool standalone)
        {
                Address address(system.local_ip(), port);
                auto socket_factory = std::make_shared<WebSocketFactory>(log, system);
                std::unique_ptr<IServerSocket> server_socket
                        = std::make_unique<ServerSocket>(log, system, address);
                std::unique_ptr<IWebSocketServer> ws_server
                        = std::make_unique<WebSocketServer>(server_socket, socket_factory,
                                                            listener, log);
                auto hub = std::make_unique<MessageHub>(topic, type, ws_server,
                                                        socket_factory, log, system);
                if (!standalone) {
                        hub->register_topic();
                        hub->start_register_thread();
                }
                return hub;
        }

        std::unique_ptr<IMessageHub> MessageHub::create(const std::string& topic,
                                                        const std::string& type,
                                                        IMessageListener& listener,
                                                        ILog& log, ISystem& system)
        {
                return create(topic, type, listener, log, system, 0, false);
        }

        MessageHub::MessageHub(const std::string &topic,
                               const std::string &type,
                               std::unique_ptr<IWebSocketServer>& server_socket,
                               const std::shared_ptr<IWebSocketFactory>& socket_factory,
                               ILog& log, ISystem& system)
                : server_(std::move(server_socket)),
                  socket_factory_(socket_factory),
                  topic_(topic),
                  type_(type),
                  log_(log),
                  system_(system),
                  thread_(),
                  done_(false)
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
                //register_topic();
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

                RegistryProxy registry(registry_socket, system_, log_);

                Address my_address;
                server_->get_address(my_address);

                registry.set(topic_, my_address, type_);
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

        void MessageHub::start_register_thread()
        {
                done_ = false;
                thread_ = std::thread(&rcom::MessageHub::update_register, this);
        }
        
        void MessageHub::stop_register_thread()
        {
                done_ = true;
                if (thread_.joinable())
                        thread_.join();
        }
        
        void MessageHub::update_register()
        {
                while (!done_) {
                        try {
                                register_topic();
                        } catch (const std::exception& ex) {
                        } catch (...) {
                                log_err(log_, "MessageHub::update_register: "
                                        "caught unknown excpetion");
                        }
                        system_.sleep(3);
                }
        }
}
