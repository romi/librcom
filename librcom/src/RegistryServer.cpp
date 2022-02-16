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
#include "RegistryServer.h"
#include "Address.h"

namespace rcom {

        static Address registry_address_(10101);
        
        void RegistryServer::get_address(IAddress& address)
        {
                // FIXME
                std::string address_string;
                registry_address_.tostring(address_string);
                address.set(address_string);
        }
        
        void RegistryServer::set_address(const char *ip, uint16_t port)
        {
                registry_address_.set(ip, port);
        }

        //
        
        RegistryServer::RegistryServer(IRegistry& registry)
                : registry_(registry), response_()
        {
        }
        
        RegistryServer::~RegistryServer()
        {
        }

        bool RegistryServer::set(const std::string& topic, IAddress& address)
        {
                return registry_.set(topic, address);
        }
                        
        bool RegistryServer::get(const std::string& topic, IAddress& address)
        {
                return registry_.get(topic, address);
        }
        
        bool RegistryServer::remove(const std::string& topic)
        {
                return registry_.remove(topic);
        }        
        
        void RegistryServer::onmessage(IWebSocket& websocket,
                                       rcom::MemBuffer& message,
                                       MessageType type)
        {
                (void) type;
                log_info("RegistryServer: Received message: %s", message.tostring().c_str());
                
                try {
                        handle_message(websocket, message);

                } catch (nlohmann::json::exception& je) {
                        log_error("RegistryServer: json error: %s", je.what());
                        send_fail(websocket, je.what());
                        
                } catch (std::runtime_error& re) {
                        log_error("RegistryServer: runtime error: %s", re.what());
                        send_fail(websocket, re.what());
                        
                } catch (...) {
                        log_error("RegistryServer: exception");
                        send_fail(websocket, "Internal error");
                }                    
        }
        
        void RegistryServer::handle_message(IWebSocket& websocket,
                                            rcom::MemBuffer& message)
        {
                nlohmann::json obj = nlohmann::json::parse(message.tostring());
                handle_json_message(websocket, obj);
        }

        void RegistryServer::handle_json_message(IWebSocket& websocket, nlohmann::json& message)
        {
                std::string request = message["request"];
                if (request == "register") {
                        handle_register(websocket, message);
                
                } else if (request == "unregister") {
                        handle_unregister(websocket, message);
                                
                } else if (request == "get") {
                        handle_get(websocket, message);                
                
                } else {
                        log_warning("Unknown request: %s", request.c_str());
                        send_fail(websocket, "Unknown request");
                }
        }
        
        void RegistryServer::handle_register(IWebSocket& websocket, nlohmann::json& message)
        {
                std::string topic = message["topic"];
                std::string address_string = message["address"];
                Address address(address_string);
                
                if (set(topic, address)) {
                        send_success(websocket);
                        log_info("RegistryServer: Register topic '%s' at %s",
                               topic.c_str(), address_string.c_str());
                } else {
                        send_fail(websocket, "set() failed");
                }
        }

        void RegistryServer::handle_unregister(IWebSocket& websocket, nlohmann::json& message)
        {
                std::string topic = message["topic"];
                
                if (remove(topic)) {
                        send_success(websocket);
                        log_info("RegistryServer: Unregister topic '%s'", topic.c_str());
                } else {
                        send_fail(websocket, "remove() failed");
                }
        }

        void RegistryServer::handle_get(IWebSocket& websocket, nlohmann::json& message)
        {
                std::string topic = message["topic"];
                Address address;
                
                if (get(topic, address)) {
                        send_address(websocket, address);
                        log_info("RegistryServer: Get topic '%s'", topic.c_str());
                } else {
                        log_info("RegistryServer: Get topic '%s' failed", topic.c_str());
                        send_fail(websocket, "No such topic");
                }
        }

        void RegistryServer::send_address(IWebSocket& websocket, IAddress& address)
        {
                std::string address_string;
                address.tostring(address_string);
                        
                response_.clear();
                response_.printf(R"({"success":true, "address": "%s"})",
                                 address_string.c_str());
                send_response(websocket);
        }

        void RegistryServer::send_fail(IWebSocket& websocket, const std::string& message)
        {
                log_warning("RegistryServer: Failure: %s", message.c_str());
                response_.clear();
                response_.printf(R"({"success":false, "message":"%s"})",
                                 message.c_str());
                send_response(websocket);
        }

        void RegistryServer::send_success(IWebSocket& websocket)
        {
                response_.clear();
                response_.printf(R"({"success":true})");
                send_response(websocket);
        }

        void RegistryServer::send_response(IWebSocket& websocket)
        {
                if (!websocket.send(response_, kTextMessage)) {
                        log_error("RegistryServer: IWebSocket.send failed");
                }
        }
}
