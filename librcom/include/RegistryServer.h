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
#ifndef _LIBRCOM_REGISTRY_SERVER_H_
#define _LIBRCOM_REGISTRY_SERVER_H_

#include <MemBuffer.h>
#include <JsonCpp.h>
#include "IRegistry.h"
#include "ISocketFactory.h"
#include "IMessageHub.h"
#include "IWebSocketServer.h"
#include "IMessageListener.h"

namespace rcom {

        class RegistryServer : public IMessageListener
        {
        public:
                static void get_address(IAddress& address);
                static void set_address(const char *ip, uint16_t port = 10101);        
                
        protected:
                IRegistry& registry_;
                rpp::MemBuffer response_;
                
                bool set(const std::string& topic, IAddress& address); 
                bool get(const std::string& topic, IAddress& address);
                bool remove(const std::string& topic);

                void handle_message(IWebSocket& websocket, rpp::MemBuffer& message);
                void handle_json_message(IWebSocket& websocket, JsonCpp& message);
                void handle_register(IWebSocket& websocket, JsonCpp& message);
                void handle_unregister(IWebSocket& websocket, JsonCpp& message);
                void handle_get(IWebSocket& websocket, JsonCpp& message);
                void send_address(IWebSocket& websocket, IAddress& address);
                void send_fail(IWebSocket& websocket, const std::string& message);
                void send_success(IWebSocket& websocket);
                void send_response(IWebSocket& websocket);
                
        public:
                RegistryServer(IRegistry& registry);
                virtual ~RegistryServer() override;
                
                void onmessage(IWebSocket& websocket,
                               rpp::MemBuffer& message,
                               MessageType type) override;                
        };
}

#endif // _LIBRCOM_REGISTRY_SERVER_H_

