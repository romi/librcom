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
#include "RawMessageHub.h"
#include "Address.h"
#include "ClientSideWebSocket.h"
#include "WebSocketServer.h"
#include "RegistryServer.h"
#include "RegistryProxy.h"
#include "ServerSocket.h"
#include "util.h"
#include "DummyMessageListener.h"

namespace rcom {

        RawMessageHub::RawMessageHub(const std::string& topic,
                                     const std::shared_ptr<IMessageListener>& listener)
                : RawMessageHub(topic, listener, 0)
        {

        }

        RawMessageHub::RawMessageHub(const std::string &topic,
                                     const std::shared_ptr<IMessageListener> &listener,
                                     uint16_t port)
                : server_(),
                  topic_(topic)
        {
                if (!is_valid_topic(topic)) {
                        r_err("RawMessageHub: Invalid topic: %s", topic.c_str());
                        throw std::runtime_error("RawMessageHub: Invalid topic");
                }

                BuildWebServerSocket(listener, port);

        }

        void RawMessageHub::BuildWebServerSocket(const std::shared_ptr<IMessageListener> &listener, uint16_t port)
        {
                Address address(port);
                std::unique_ptr<rpp::ILinux> linux
                        = std::make_unique<rpp::Linux>();

                std::unique_ptr<IServerSocket> server_socket
                        = std::make_unique<ServerSocket>(linux, address);

                std::shared_ptr<ISocketFactory> factory
                        = std::make_shared<SocketFactory>();

                server_ = std::make_unique<WebSocketServer>(server_socket,
                                                            factory,
                                                            listener);
        }

        RawMessageHub::RawMessageHub(const std::string& topic)
                : RawMessageHub(topic, std::make_shared<DummyMessageListener>())
        {
        }

        std::string& RawMessageHub::topic()
        {
                return topic_;
        }

        bool RawMessageHub::register_topic()
        {
                Address registry_address;
                RegistryServer::get_address(registry_address);

                SocketFactory factory;

                std::unique_ptr<IWebSocket> registry_socket
                        = factory.new_client_side_websocket(registry_address);

                RegistryProxy registry(registry_socket);

                Address my_address;
                server_->get_address(my_address);

                return registry.set(topic_, my_address);
        }

        void RawMessageHub::handle_events()
        {
                server_->handle_events();
        }

        void RawMessageHub::broadcast(rpp::MemBuffer &message,
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
