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
#include <algorithm>
#include "rcom/ConsoleLogger.h"
#include "rcom/RegistryProxy.h"
#include "rcom/util.h"

namespace rcom {

        RegistryProxy::RegistryProxy(std::unique_ptr<IWebSocket>& websocket)
                : websocket_()
        {
                websocket_ = std::move(websocket);
        }

        RegistryProxy::~RegistryProxy()
        {
                websocket_->close(kCloseNormal);
        }
        
        bool RegistryProxy::set(const std::string& topic, IAddress& address)
        {
                rcom::MemBuffer request;
                make_register_request(request, topic, address);
                bool success = (send_request(request)
                                && response_is_success("register"));
                return success;
        }

        void RegistryProxy::make_register_request(rcom::MemBuffer& request,
                                                  const std::string& topic,
                                                  IAddress& address)
        {
                std::string address_string;
                address.tostring(address_string);
                request.printf("{\"request\": \"register\", "
                               "\"topic\": \"%s\", \"address\": \"%s\"}",
                               topic.c_str(), address_string.c_str());
        }
        
        bool RegistryProxy::get(const std::string& topic, IAddress& address, double timeout)
        {
                bool success = false;
                bool timed_out = false;
                rcom::MemBuffer request;
                ILinux& linux = websocket_->get_linux();
                double start_time = rcom_time(linux);

                make_get_request(request, topic);
                
                while (!success && !timed_out) {
                        
                        success = (send_request(request)
                                   && read_address(address));
                        
                        double now = rcom_time(linux);
                        double time_passed = now - start_time;
                        timed_out = (time_passed >= timeout);
                        
                        if (!success && !timed_out) {
                                double duration = std::min(0.5, timeout - time_passed);
                                rcom_sleep(linux, duration);
                        }
                }
                return success;
        }
        
        void RegistryProxy::make_get_request(rcom::MemBuffer& request,
                                             const std::string& topic)
        {
                request.printf("{\"request\": \"get\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
        
        bool RegistryProxy::remove(const std::string& topic)
        {
                rcom::MemBuffer request;
                make_unregister_request(request, topic);
                bool success = (send_request(request)
                                && response_is_success("unregister"));
                return success;
        }

        void RegistryProxy::make_unregister_request(rcom::MemBuffer& request,
                                                    const std::string& topic)
        {
                request.printf("{\"request\": \"unregister\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
                
        bool RegistryProxy::send_request(rcom::MemBuffer& request)
        {
                return websocket_->send(request, kTextMessage);
        }

        bool RegistryProxy::response_is_success(const std::string& method)
        {
                bool success = false;
                rcom::MemBuffer response;
                if (read_response(response)) {
                        nlohmann::json json = parse_response(response);
                        success = is_success(json);
                        if (!success) {
                                print_error(json, method);
                        }
                }
                return success;
        }

        bool RegistryProxy::read_response(rcom::MemBuffer& response)
        {
                bool success = false;
                RecvStatus status;
                status = websocket_->recv(response, 2.0);
                if (status == kRecvText)
                        success = true;
                else if (status == kRecvTimeOut)
                        log_warning("RegistryProxy::read_response: Time-out");
                else if (status == kRecvError)
                        log_warning("RegistryProxy::read_response: Error");
                return success;
        }
        
        nlohmann::json RegistryProxy::parse_response(rcom::MemBuffer& response)
        {
                nlohmann::json json;
                std::string text = response.tostring();
                try {
                        json = nlohmann::json::parse(text.c_str());
                } catch (nlohmann::json::exception& jerr) {
                        log_error("RegistryProxy: %s", jerr.what());
                }
                return json;
        }
        
        bool RegistryProxy::is_success(nlohmann::json& jsonobj)
        {
                bool retval = false;
                try {
                        retval = jsonobj["success"];
                } catch (nlohmann::json::exception& jerr) {
                        log_error("RegistryProxy: %s", jerr.what());
                }
                return retval;
        }
        
        void RegistryProxy::print_error(nlohmann::json& jsonobj, const std::string& method)
        {
                log_error("RegistryProxy: %s: Request failed", method.c_str());
                try {
                        log_error("RegistryProxy: Reason: %s",
                                  jsonobj["message"].dump().c_str());
                } catch (nlohmann::json::exception& jerr) {
                        log_error("RegistryProxy: %s", jerr.what());
                }
        }
                                
        bool RegistryProxy::read_address(IAddress& address)
        {
                bool success = false;
                rcom::MemBuffer response;
                if (read_response(response)) {
                        nlohmann::json jsonobj = parse_response(response);
                        if (is_success(jsonobj)) {
                                std::string address_string;
                                if (get_address(jsonobj, address_string)) {
                                        success = address.set(address_string);
                                }
                        }
                }
                return success;
        }
        
        bool RegistryProxy::get_address(nlohmann::json& jsonobj,
                                        std::string& address_string)
        {
                bool success = false;
                try {
                        address_string = jsonobj["address"];
                        success = true;
                } catch (nlohmann::json::exception& jerr) {
                        log_error("RegistryProxy: %s", jerr.what());
                }
                return success;
        }
}

