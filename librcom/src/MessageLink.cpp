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
#include "rcom/MessageLink.h"
#include "rcom/Address.h"
#include "rcom/RegistryProxy.h"
#include "rcom/RegistryServer.h"
#include "rcom/util.h"
#include "rcom/ConsoleLog.h"
#include "rcom/Linux.h"
#include "rcom/SocketFactory.h"

namespace rcom {
        
        std::unique_ptr<IMessageLink> MessageLink::create(const std::string& topic,
                                                          double timeout)
        {
                std::shared_ptr<ILog> log = std::make_shared<ConsoleLog>();
                return create(topic, timeout, log);
        }

        std::unique_ptr<IMessageLink> MessageLink::create(const std::string& topic,
                                                          double timeout,
                                                          const std::shared_ptr<ILog>& log)
        {
                std::shared_ptr<ILinux> linux = std::make_shared<Linux>();                
                std::shared_ptr<ISocketFactory> factory
                        = std::make_shared<SocketFactory>(linux, log);
                return std::make_unique<MessageLink>(topic, timeout, factory, linux, log);
        }

        MessageLink::MessageLink(const std::string& topic,
                                 double timeout,
                                 const std::shared_ptr<ISocketFactory>& factory,
                                 const std::shared_ptr<ILinux>& linux,
                                 const std::shared_ptr<ILog>& log)
                : factory_(factory),
                  websocket_(),
                  topic_(topic),
                  recv_status_(kRecvText),
                  linux_(linux),
                  log_(log)
        {
                if (!is_valid_topic(topic_)) {
                        log_err(log_, "MessageLink: Ill-formatted topic string: %s",
                                topic.c_str());
                        throw std::runtime_error("MessageLink: Ill-formatted topic string");
                }
                
                if (!connect(timeout)) {
                        log_err(log_, "MessageLink: Failed to connect: %s",
                                topic.c_str());
                        throw std::runtime_error("MessageLink: Failed to connect");
                }
        }

        MessageLink::~MessageLink()
        {
                websocket_->close(kCloseGoingAway);
        }

        bool MessageLink::connect(double timeout)
        {
                bool success = false;

                Address hub_address;
                if (get_remote_address(hub_address, timeout)) {
                        websocket_ = factory_->new_client_side_websocket(hub_address);
                        success = true;
                } else {
                        log_warn(log_, "MessageLink::connect: Failed to obtain address for "
                                 "topic '%s'", topic_.c_str());
                }
                return success;
        }

        bool MessageLink::get_remote_address(Address& address, double timeout)
        {
                Address registry_address;
                RegistryServer::get_address(registry_address);
                std::unique_ptr<IWebSocket> registry_socket
                        = factory_->new_client_side_websocket(registry_address);
                RegistryProxy registry(registry_socket, linux_, log_);
                return registry.get(topic_, address, timeout);
        }

        std::string& MessageLink::get_topic()
        {
                return topic_;
        }
        
        bool MessageLink::recv(MemBuffer& message, double timeout)
        {
                recv_status_ = websocket_->recv(message, timeout);
                return obtained_message();
        }

        bool MessageLink::obtained_message()
        {
                return (recv_status_ == kRecvText
                        || recv_status_ == kRecvBinary);
        }
        
        RecvStatus MessageLink::recv_status()
        {
                return recv_status_;
        }

        bool MessageLink::send(MemBuffer& message, MessageType type)
        {
                return websocket_->send(message, type);
        }

        bool MessageLink::is_connected()
        {
                return websocket_->is_connected();
        }
}
