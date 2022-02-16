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
#include "ConsoleLogger.h"
#include <algorithm>
#include <utility>
#include "RawMessageHub.h"
#include "Address.h"
#include "RegistryServer.h"
#include "RegistryProxy.h"
#include "ServerSocket.h"
#include "util.h"
#include "DummyMessageListener.h"
#include "IWebSocketServerFactory.h"

namespace rcom {

        RawMessageHub::RawMessageHub(const std::string& topic,
                                     const std::shared_ptr<IMessageListener>& listener,
                                     const std::shared_ptr<ISocketFactory>& socketFactory,
                                     const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory)
                : RawMessageHub(topic, listener, socketFactory, webSocketServerFactory, 0)
        {

        }

        RawMessageHub::RawMessageHub(const std::string &topic,
                                     const std::shared_ptr<IMessageListener> &listener,
                                     const std::shared_ptr<ISocketFactory>&  socketFactory,
                                     const std::shared_ptr<IWebSocketServerFactory>& webSocketServerFactory,
                                     uint16_t port)
                : server_(),
                  socketFactory_(socketFactory),
                  topic_(topic)
        {
            try {
                if (nullptr == socketFactory_)
                {
                    throw std::invalid_argument("socketFactory_");
                }
                if (nullptr == webSocketServerFactory)
                {
                    throw std::invalid_argument("webSocketServerFactory");
                }
                if (!is_valid_topic(topic)) {
                    std::string error("topic: ");
                    error += topic;
                    throw std::invalid_argument(error.c_str());
                }
                server_ = webSocketServerFactory->new_web_socket_server(listener, port);
                if (nullptr == server_)
                {
                    throw std::invalid_argument("server_");
                }
            }
            catch (std::invalid_argument& e){
                log_error("RawMessageHub: Invalid argument: %s", e.what());
                throw;
            }
        }

        std::string& RawMessageHub::topic()
        {
                return topic_;
        }

        bool RawMessageHub::register_topic()
        {
                Address registry_address;
                RegistryServer::get_address(registry_address);

                std::unique_ptr<IWebSocket> registry_socket
                        = socketFactory_->new_client_side_websocket(registry_address);

                RegistryProxy registry(registry_socket);

                Address my_address;
                server_->get_address(my_address);

                return registry.set(topic_, my_address);
        }

        void RawMessageHub::handle_events()
        {
                server_->handle_events();
        }

        void RawMessageHub::broadcast(rcom::MemBuffer &message,
                                      MessageType type,
                                      IWebSocket *exclude)
        {
                server_->broadcast(message, type, exclude);
        }

        size_t RawMessageHub::count_links()
        {
                return server_->count_links();
        }

}
