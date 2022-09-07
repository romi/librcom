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
#ifndef _LIBRCOM_REGISTRYSERVER_H
#define _LIBRCOM_REGISTRYSERVER_H

#include "rcom/MemBuffer.h"
#include "rcom/json.hpp"
#include "rcom/IRegistry.h"
#include "rcom/ISocketFactory.h"
#include "rcom/IMessageHub.h"
#include "rcom/IWebSocketServer.h"
#include "rcom/IMessageListener.h"

namespace rcom {

        class RegistryServer : public IMessageListener
        {
        public:
                static void get_address(IAddress& address);
                static void set_address(const char *ip, uint16_t port = 10101);        
                
        protected:
                IRegistry& registry_;
                std::shared_ptr<ILog> log_;
                MemBuffer response_;
                
                void set(const std::string& topic, IAddress& address); 
                bool get(const std::string& topic, IAddress& address);
                void remove(const std::string& topic);

                void handle_message(IWebSocket& websocket, MemBuffer& message);
                void handle_json_message(IWebSocket& websocket, nlohmann::json& message);
                void handle_register(IWebSocket& websocket, nlohmann::json& message);
                void handle_unregister(IWebSocket& websocket, nlohmann::json& message);
                void handle_get(IWebSocket& websocket, nlohmann::json& message);
                void send_address(IWebSocket& websocket, const std::string& topic, IAddress& address);
                void send_empty_address(IWebSocket& websocket);
                void send_fail(IWebSocket& websocket, const std::string& message);
                void send_success(IWebSocket& websocket);
                void send_response(IWebSocket& websocket);
                
        public:
                RegistryServer(IRegistry& registry,
                               const std::shared_ptr<ILog>& log);
                virtual ~RegistryServer() override;
                
                void onmessage(IWebSocketServer& server,
                               IWebSocket& websocket,
                               MemBuffer& message,
                               MessageType type) override;                
        };
}

#endif // _LIBRCOM_REGISTRYSERVER_H

