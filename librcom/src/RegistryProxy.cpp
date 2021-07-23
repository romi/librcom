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
#include <r.h>
#include <ClockAccessor.h>
#include "RegistryProxy.h"

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
                rpp::MemBuffer request;
                make_register_request(request, topic, address);
                bool success = (send_request(request)
                                && response_is_success("register"));
                return success;
        }

        void RegistryProxy::make_register_request(rpp::MemBuffer& request,
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
                rpp::MemBuffer request;
                double start_time = rpp::ClockAccessor::GetInstance()->time();

                make_get_request(request, topic);
                
                while (!success && !timed_out) {
                        
                        success = (send_request(request)
                                   && read_address(address));
                        
                        double now = rpp::ClockAccessor::GetInstance()->time();
                        double time_passed = now - start_time;
                        timed_out = (time_passed >= timeout);
                        
                        if (!success && !timed_out) {
                                double duration = std::min(0.5, timeout - time_passed);
                                rpp::ClockAccessor::GetInstance()->sleep(duration);
                        }
                }
                return success;
        }
        
        void RegistryProxy::make_get_request(rpp::MemBuffer& request,
                                             const std::string& topic)
        {
                request.printf("{\"request\": \"get\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
        
        bool RegistryProxy::remove(const std::string& topic)
        {
                rpp::MemBuffer request;
                make_unregister_request(request, topic);
                bool success = (send_request(request)
                                && response_is_success("unregister"));
                return success;
        }

        void RegistryProxy::make_unregister_request(rpp::MemBuffer& request,
                                                    const std::string& topic)
        {
                request.printf("{\"request\": \"unregister\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
                
        bool RegistryProxy::send_request(rpp::MemBuffer& request)
        {
                return websocket_->send(request, kTextMessage);
        }

        bool RegistryProxy::response_is_success(const std::string& method)
        {
                bool success = false;
                rpp::MemBuffer response;
                if (read_response(response)) {
                        JsonCpp json = parse_response(response);
                        success = is_success(json);
                        if (!success) {
                                print_error(json, method);
                        }
                }
                return success;
        }

        bool RegistryProxy::read_response(rpp::MemBuffer& response)
        {
                bool success = false;
                RecvStatus status;
                status = websocket_->recv(response, 2.0);
                if (status == kRecvText)
                        success = true;
                else if (status == kRecvTimeOut)
                        r_warn("RegistryProxy::read_response: Time-out");
                else if (status == kRecvError)
                        r_warn("RegistryProxy::read_response: Error");
                return success;
        }
        
        JsonCpp RegistryProxy::parse_response(rpp::MemBuffer& response)
        {
                JsonCpp json;                
                std::string text = response.tostring();
                try {
                        json = JsonCpp::parse(text.c_str());
                } catch (JSONError& jerr) {
                        r_err("RegistryProxy: %s", jerr.what());
                }
                return json;
        }
        
        bool RegistryProxy::is_success(JsonCpp& json)
        {
                bool retval = false;
                try {
                        retval = json.boolean("success");
                } catch (JSONError& jerr) {
                        r_err("RegistryProxy: %s", jerr.what());
                }
                return retval;
        }
        
        void RegistryProxy::print_error(JsonCpp& json, const std::string& method)
        {
                r_err("RegistryProxy: %s: Request failed", method.c_str());
                try {
                        r_err("RegistryProxy: Reason: %s", json.str("message"));
                } catch (JSONError& jerr) {
                        r_err("RegistryProxy: %s", jerr.what());
                }
        }
                                
        bool RegistryProxy::read_address(IAddress& address)
        {
                bool success = false;
                rpp::MemBuffer response;
                if (read_response(response)) {
                        JsonCpp json = parse_response(response);
                        if (is_success(json)) {
                                std::string address_string;
                                if (get_address(json, address_string)) {
                                        success = address.set(address_string);
                                }
                        }
                }
                return success;
        }
        
        bool RegistryProxy::get_address(JsonCpp& json, std::string& address_string)
        {
                bool success = false;
                try {
                        address_string = json.str("address");
                        success = true;
                } catch (JSONError& jerr) {
                        r_err("RegistryProxy: %s", jerr.what());
                }
                return success;
        }
}

