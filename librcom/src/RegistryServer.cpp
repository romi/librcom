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
#include "rcom/RegistryServer.h"
#include "rcom/Address.h"
#include "rcom/RegistryLookup.h"
#include "rcom/util.h"

namespace rcom {

        static bool use_registry_address_ = false;
        static Address registry_address_;

        void RegistryServer::get_address(IAddress& address)
        {
                // FIXME
                std::string address_string;
                if (use_registry_address_) {
                        address_string = registry_address_.tostring();
                } else {
                        RegistryLookup lookup(kLookupPort);
                        address_string = lookup.lookup(); // Hmmm...                        
                }
                address.set(address_string);
        }
        
        void RegistryServer::set_address(const char *ip)
        {
                registry_address_.set(ip, kRegistryPort);
                use_registry_address_ = true;
        }

        //
        
        RegistryServer::RegistryServer(IRegistry& registry, ILog& log)
                : registry_(registry),
                  log_(log),
                  response_()
        {
        }
        
        RegistryServer::~RegistryServer()
        {
        }

        void RegistryServer::set(const std::string& topic, IAddress& address,
                                 const std::string& type)
        {
                registry_.set(topic, address, type);
        }
                        
        bool RegistryServer::get(const std::string& topic, IAddress& address)
        {
                return registry_.get(topic, address, 12.0);
        }
        
        void RegistryServer::remove(const std::string& topic)
        {
                registry_.remove(topic);
        }        
        
        void RegistryServer::onmessage(IWebSocketServer& server,
                                       IWebSocket& websocket,
                                       MemBuffer& message,
                                       MessageType type)
        {
                (void) server;
                (void) type;
                
                log_info(log_, "RegistryServer: Received message: %s",
                         message.tostring().c_str());
                
                try {
                        handle_message(websocket, message);

                } catch (nlohmann::json::exception& je) {
                        log_err(log_, "RegistryServer: json error: %s", je.what());
                        send_fail(websocket, je.what());
                        
                } catch (std::runtime_error& re) {
                        log_err(log_, "RegistryServer: runtime error: %s", re.what());
                        send_fail(websocket, re.what());
                        
                } catch (...) {
                        log_err(log_, "RegistryServer: exception");
                        send_fail(websocket, "Internal error");
                }                    
        }

        void RegistryServer::assert_topic(const std::string& topic)
        {
                if (!is_valid_topic(topic)) {
                        throw std::runtime_error("Invalid topic string");
                }
        }
        
        void RegistryServer::assert_type(const std::string& topic)
        {
                if (!is_valid_name(topic)) {
                        throw std::runtime_error("Invalid type string");
                }
        }
        
        void RegistryServer::handle_message(IWebSocket& websocket,
                                            MemBuffer& message)
        {
                nlohmann::json obj = nlohmann::json::parse(message.tostring());
                handle_json_message(websocket, obj);
        }

        void RegistryServer::handle_json_message(IWebSocket& websocket,
                                                 nlohmann::json& message)
        {
                std::string request = message["request"];
                if (request == "register") {
                        handle_register(websocket, message);
                
                } else if (request == "unregister") {
                        handle_unregister(websocket, message);
                                
                } else if (request == "get") {
                        handle_get(websocket, message);                
                
                } else if (request == "list") {
                        handle_list(websocket, message);                
                
                } else {
                        log_warn(log_, "Unknown request: %s", request.c_str());
                        send_fail(websocket, "Unknown request");
                }
        }
        
        void RegistryServer::handle_register(IWebSocket& websocket,
                                             nlohmann::json& message)
        {
                try {
                        std::string topic = message["topic"];
                        std::string address_string = message["address"];
                        std::string type = message["type"];
                        Address address(address_string);

                        assert_topic(topic);
                        assert_type(topic);
                        
                        set(topic, address, type);
                        send_success(websocket);
                        log_info(log_, "RegistryServer: Register topic '%s' at %s",
                                 topic.c_str(), address_string.c_str());
                        
                } catch (std::exception& e) {
                        log_err(log_, "RegistryServer: handle_register: %s", e.what());
                        send_fail(websocket, "RegistryServer: set failed");
                }
        }

        void RegistryServer::handle_unregister(IWebSocket& websocket,
                                               nlohmann::json& message)
        {
                try {
                        std::string topic = message["topic"];
                        remove(topic);
                        send_success(websocket);
                        log_info(log_, "RegistryServer: Unregister topic '%s'",
                                 topic.c_str());
                } catch (std::exception& e) {
                        log_err(log_, "RegistryServer: handle_unregister: %s", e.what());
                        send_fail(websocket, "RegistryServer: remove failed");
                }
        }

        void RegistryServer::handle_get(IWebSocket& websocket, nlohmann::json& message)
        {
                try {
                        Address address;
                        std::string topic = message["topic"];
                        
                        if (get(topic, address)) {
                                send_address(websocket, topic, address);
                                log_info(log_, "RegistryServer: Get topic '%s'",
                                         topic.c_str());
                        } else {
                                send_empty_address(websocket);
                                log_info(log_, "RegistryServer: No such topic: '%s'",
                                         topic.c_str());
                        }
                } catch (std::runtime_error& e) {
                        log_info(log_, "RegistryServer: Get topic failed: %s", e.what());
                        send_fail(websocket, e.what());
                }
        }

        void RegistryServer::handle_list(IWebSocket& websocket, nlohmann::json& /*message*/)
        {
                try {
                        std::vector<RegistryEntry> list;
                        registry_.list(list);

                        nlohmann::json array = nlohmann::json::array();

                        for (size_t i = 0; i < list.size(); i++) {
                                RegistryEntry& entry = list[i];
                                nlohmann::json obj = {{ "topic", entry.topic },
                                                      { "address", entry.address.tostring() },
                                                      { "type", entry.type }};
                                array[i] = obj; 
                        }

                        nlohmann::json result = {{"success", true}, {"list", array}};
                        
                        response_.clear();
                        response_.append(result.dump());
                        send_response(websocket);
                        
                } catch (std::runtime_error& e) {
                        log_info(log_, "RegistryServer: Get topic failed: %s", e.what());
                        send_fail(websocket, e.what());
                }
        }

        void RegistryServer::send_address(IWebSocket& websocket, const std::string& topic,
                                          IAddress& address)
        {
                std::string address_string = address.tostring();
                        
                response_.clear();
                response_.printf(R"({"success":true, "topic": "%s", "address": "%s"})",
                                 topic.c_str(), address_string.c_str());
                send_response(websocket);
        }

        void RegistryServer::send_empty_address(IWebSocket& websocket)
        {
                response_.clear();
                response_.printf(R"({"success":true})");
                send_response(websocket);
        }

        void RegistryServer::send_fail(IWebSocket& websocket, const std::string& message)
        {
                log_warn(log_, "RegistryServer: Failure: %s", message.c_str());
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
                        log_err(log_, "RegistryServer: IWebSocket.send failed");
                }
        }
}
