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
                                       rpp::MemBuffer& message,
                                       MessageType type)
        {
                (void) type;
                r_info("RegistryServer: Received message: %s", message.tostring().c_str());
                
                try {
                        handle_message(websocket, message);

                } catch (JSONError& je) {
                        r_err("RegistryServer: json error: %s", je.what());
                        send_fail(websocket, je.what());
                        
                } catch (std::runtime_error& re) {
                        r_err("RegistryServer: runtime error: %s", re.what());
                        send_fail(websocket, re.what());
                        
                } catch (...) {
                        r_err("RegistryServer: exception");
                        send_fail(websocket, "Internal error");
                }                    
        }
        
        void RegistryServer::handle_message(IWebSocket& websocket,
                                            rpp::MemBuffer& message)
        {
                JsonCpp obj = JsonCpp::parse(message);
                handle_json_message(websocket, obj);
        }

        void RegistryServer::handle_json_message(IWebSocket& websocket, JsonCpp& message)
        {
                std::string request = message.str("request");
                if (request.compare("register") == 0) {
                        handle_register(websocket, message);
                
                } else if (request.compare("unregister") == 0) {
                        handle_unregister(websocket, message);
                                
                } else if (request.compare("get") == 0) {
                        handle_get(websocket, message);                
                
                } else {
                        r_warn("Unknown request: %s", request.c_str());
                        send_fail(websocket, "Unknown request");
                }
        }
        
        void RegistryServer::handle_register(IWebSocket& websocket, JsonCpp& message)
        {
                std::string topic = message.str("topic");
                std::string address_string = message.str("address");
                Address address(address_string);
                
                if (set(topic, address)) {
                        send_success(websocket);
                        r_info("RegistryServer: Register topic '%s' at %s",
                               topic.c_str(), address_string.c_str());
                } else {
                        send_fail(websocket, "set() failed");
                }
        }

        void RegistryServer::handle_unregister(IWebSocket& websocket, JsonCpp& message)
        {
                std::string topic = message.str("topic");
                
                if (remove(topic)) {
                        send_success(websocket);
                        r_info("RegistryServer: Unregister topic '%s'", topic.c_str());
                } else {
                        send_fail(websocket, "remove() failed");
                }
        }

        void RegistryServer::handle_get(IWebSocket& websocket, JsonCpp& message)
        {
                std::string topic = message.str("topic");
                Address address;
                
                if (get(topic, address)) {
                        send_address(websocket, address);
                        r_info("RegistryServer: Get topic '%s'", topic.c_str());
                } else {
                        r_info("RegistryServer: Get topic '%s' failed", topic.c_str());
                        send_fail(websocket, "No such topic");
                }
        }

        void RegistryServer::send_address(IWebSocket& websocket, IAddress& address)
        {
                std::string address_string;
                address.tostring(address_string);
                        
                response_.clear();
                response_.printf("{\"success\":true, \"address\": \"%s\"}",
                                 address_string.c_str());
                send_response(websocket);
        }

        void RegistryServer::send_fail(IWebSocket& websocket, const std::string& message)
        {
                r_warn("RegistryServer: Failure: %s: %s", message.c_str());
                response_.clear();
                response_.printf("{\"success\":false, \"message\":\"%s\"}",
                                 message.c_str());
                send_response(websocket);
        }

        void RegistryServer::send_success(IWebSocket& websocket)
        {
                response_.clear();
                response_.printf("{\"success\":true}");
                send_response(websocket);
        }

        void RegistryServer::send_response(IWebSocket& websocket)
        {
                if (!websocket.send(response_)) {
                        r_err("RegistryServer: IWebSocket.send failed");
                }
        }
}
