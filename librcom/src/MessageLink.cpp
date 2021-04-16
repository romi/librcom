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
#include "MessageLink.h"
#include <Address.h>
#include <RegistryProxy.h>
#include <RegistryServer.h>
#include "Clock.h"
#include "util.h"

namespace rcom {

        MessageLink::MessageLink(const std::string& topic)
                : linux_(),
                  clock_(),
                  factory_(linux_, clock_),
                  websocket_(),
                  topic_(topic),
                  recv_status_(kRecvText)
        {
                if (!is_valid_topic(topic_)) {
                        r_err("MessageLink: Ill-formatted topic string: %s", topic.c_str());
                        throw std::runtime_error("MessageLink: Ill-formatted topic string");
                }
                
                if (!connect()) {
                        r_err("MessageLink: Failed to connect: %s", topic.c_str());
                        throw std::runtime_error("MessageLink: Failed to connect");
                }
        }

        MessageLink::~MessageLink()
        {
                websocket_->close(kCloseGoingAway);
        }

        bool MessageLink::connect()
        {
                bool success = false;

                Address hub_address;
                if (get_remote_address(hub_address)) {
                        websocket_ = factory_.new_client_side_websocket(hub_address);
                        success = true;
                } else {
                        r_warn("MessageLink::connect: Failed to obtain address for "
                               "topic '%s'", topic_.c_str());
                }
                return success;
        }

        bool MessageLink::get_remote_address(Address& address)
        {
                Address registry_address;
                RegistryServer::get_address(registry_address);
                std::unique_ptr<IWebSocket> registry_socket
                        = factory_.new_client_side_websocket(registry_address);
                Clock clock;
                RegistryProxy registry(registry_socket, clock);
                return registry.get(topic_, address);
        }

        std::string& MessageLink::get_topic()
        {
                return topic_;
        }
        
        bool MessageLink::recv(rpp::MemBuffer& message, double timeout)
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

        bool MessageLink::send(rpp::MemBuffer& message)
        {
                return websocket_->send(message);
        }
}
