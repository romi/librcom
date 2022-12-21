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

#ifndef _LIBRCOM_RCOMMESSAGEHANDLER_H
#define _LIBRCOM_RCOMMESSAGEHANDLER_H

#include <memory>
#include "rcom/IMessageListener.h"
#include "rcom/IRPCHandler.h"

namespace rcom {
        
        class RcomMessageHandler : public IMessageListener
        {
        protected:
                IRPCHandler& handler_;

                void handle_binary_request(IWebSocket& websocket, MemBuffer& message);
                void handle_json_request(IWebSocket& websocket, MemBuffer& message);

                nlohmann::json parse_request(MemBuffer& message, RPCError& error);
                std::string get_method(nlohmann::json& request, RPCError& error);
                std::string get_id(nlohmann::json& request, RPCError& error);

                nlohmann::json construct_response(const std::string& id,
                                                  const std::string& method,
                                                  RPCError& error,
                                                  nlohmann::json& result);
                nlohmann::json construct_response(const std::string& id,
                                                  const std::string& method,
                                                  RPCError& error);
                nlohmann::json construct_response(const std::string& id,
                                                  const std::string& method,
                                                  int code, const char *message);
                
        public:
                explicit RcomMessageHandler(IRPCHandler& handler);
                ~RcomMessageHandler() override = default;

                void onmessage(IWebSocketServer& server,
                               IWebSocket& websocket,
                               MemBuffer& message,
                               MessageType type) override;
        };
}

#endif // _LIBRCOM_RCOMMESSAGEHANDLER_H
