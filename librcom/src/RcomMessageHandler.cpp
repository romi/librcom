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
#include <iostream>
#include <string.h>
#include "rcom/MessageFields.h"
#include "rcom/RcomMessageHandler.h"

namespace rcom {
        
        RcomMessageHandler::RcomMessageHandler(IRPCHandler& handler)
                : handler_(handler)
        {
        }

        void RcomMessageHandler::onmessage(IWebSocketServer& server,
                                           IWebSocket& websocket,
                                           MemBuffer& message,
                                           MessageType type)
        {
                (void) server;
                if (type == kTextMessage)
                        handle_json_request(websocket, message);
                else
                        handle_binary_request(websocket, message);
        }
        
        void RcomMessageHandler::handle_binary_request(IWebSocket& websocket,
                                                       MemBuffer& message)
        {
                MemBuffer result;
                RPCError error;
                nlohmann::json response;
                nlohmann::json request = parse_request(message, error);
                if (error.code == 0) {
                        
                        std::string id = get_id(request, error);
                        std::string method = get_method(request, error);
                        if (!method.empty()) {
                                nlohmann::json params;
                                if (request.contains(kParams))
                                        params = request[kParams];

                                handler_.execute(id, method, params, result, error);
                                
                        } else {
                                response = construct_response(id, kUnknownMethod, error);
                        }
                } else {
                        response = construct_response(kNoID, kUnknownMethod, error);
                }
 
                if (error.code == 0) {
                        websocket.send(result, kBinaryMessage);
                        
                } else {
                        MemBuffer serialised;
                        serialised.append(response.dump());
                        websocket.send(serialised, kTextMessage);
                }
        }
        
        void RcomMessageHandler::handle_json_request(IWebSocket& websocket,
                                                     MemBuffer& message)
        {
                (void) message;
                nlohmann::json result;
                RPCError error;
                nlohmann::json response;
                
                nlohmann::json request = parse_request(message, error);
                if (error.code == 0) {
                        
                        std::string id = get_id(request, error);
                        std::string method = get_method(request, error);
                        if (!method.empty()) {
                                nlohmann::json params;

                                if (request.contains(kParams))
                                        params = request[kParams];

                                handler_.execute(id, method, params, result, error);
                                response = construct_response(id, method, error, result);
                                

                                //std::cout << "RcomMessageHandler::handle_json_request: response: " << response << std::endl;
                                
                        } else {
                                response = construct_response(id, kUnknownMethod, error);
                        }
                } else {
                        response = construct_response(kNoID, kUnknownMethod, error);
                }
                
                MemBuffer serialised;
                serialised.append(response.dump());
                websocket.send(serialised, kTextMessage);
        }
        
        nlohmann::json RcomMessageHandler::parse_request(MemBuffer& message,
                                                         RPCError& error)
        {
                nlohmann::json request;
                try {
                        request = nlohmann::json::parse(message.tostring());
                        
                } catch (std::exception& e) {
                        error.code = RPCError::kParseError;
                        error.message = "Failed to parse the request";
                }
                return request;
        }

        std::string RcomMessageHandler::get_method(nlohmann::json& request, RPCError& error)
        {
                std::string value;
                try {
                        value = request[kMethod];
                } catch (std::exception& e) {
                        error.code = RPCError::kInvalidRequest;
                        error.message = "Missing method";
                }
                return value;
        }

        std::string RcomMessageHandler::get_id(nlohmann::json& request, RPCError& error)
        {
                std::string value;
                if (request.contains(kID)) {
                        try {
                                value = request[kID];
                        } catch (std::exception& e) {
                                error.code = RPCError::kInvalidRequest;
                                error.message = "Invalid ID";
                        }
                }
                return value;
        }

        /* Construct the envelope for a reponse with results, to be
         * sent back to the client.  */
        nlohmann::json RcomMessageHandler::construct_response(const std::string& id,
                                                              const std::string& method,
                                                              RPCError& error,
                                                              nlohmann::json& result)
        {
                nlohmann::json response = construct_response(id, method, error);
                if (!result.is_null()) {
                        response[kResult] = result;
                } 
                return response;
        }
        
        /* Construct the envelope for a reponse with results, to be
         * sent back to the client.  */
        nlohmann::json RcomMessageHandler::construct_response(const std::string& id,
                                                              const std::string& method,
                                                              RPCError& error)
        {
                return construct_response(id, method, error.code, error.message.c_str());
        }

        /* Construct the envelope for an error reponse to be sent back
         * to the client.  */
        nlohmann::json RcomMessageHandler::construct_response(const std::string& id,
                                                              const std::string& method,
                                                              int code, const char *message)
        {
                nlohmann::json response;
                
                if (!id.empty()) {
                        response[kID] = id;
                } else {
                        response[kID] = kNoID;
                }

                if (!method.empty()) {
                        response[kMethod] = method;
                } else {
                        response[kMethod] = kUnknownMethod;
                }
                
                if (code != 0) {
                        nlohmann::json error;
                        error[kErrorMessage] = "No message was given";
                        if (message != nullptr && strlen(message) > 0) {
                            error[kErrorMessage] = message;
                        }
                        error[kErrorCode] = code;
                        response[kError] = error;
                }

                return response;
        }
}
