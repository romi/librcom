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
#include "rcom/Log.h"
#include "rcom/RegistryProxy.h"
#include "rcom/util.h"

namespace rcom {

        RegistryProxy::RegistryProxy(std::unique_ptr<IWebSocket>& websocket,
                                     const std::shared_ptr<ILinux>& linux,
                                     const std::shared_ptr<ILog>& log)
                : websocket_(),
                  linux_(linux),
                  log_(log)
        {
                websocket_ = std::move(websocket);
        }

        RegistryProxy::~RegistryProxy()
        {
                websocket_->close(kCloseNormal);
        }
        
        void RegistryProxy::set(const std::string& topic, IAddress& address)
        {
                MemBuffer request;
                make_register_request(request, topic, address);
                send_request(request);
                response_assert_success();
        }

        void RegistryProxy::make_register_request(MemBuffer& request,
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
                bool found = false;
                bool timed_out = false;
                MemBuffer request;
                double start_time = rcom_time(*linux_);

                make_get_request(request, topic);
                
                while (!found && !timed_out) {
                        
                        send_request(request);
                        found = read_address(address);
                        
                        double now = rcom_time(*linux_);
                        double time_passed = now - start_time;
                        timed_out = (time_passed >= timeout);
                        
                        if (!found && !timed_out) {
                                double duration = std::min(0.5, timeout - time_passed);
                                rcom_sleep(*linux_, duration);
                        }
                }
                return found;
        }
        
        void RegistryProxy::make_get_request(MemBuffer& request,
                                             const std::string& topic)
        {
                request.printf("{\"request\": \"get\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
        
        void RegistryProxy::remove(const std::string& topic)
        {
                MemBuffer request;
                make_unregister_request(request, topic);
                send_request(request);
                response_assert_success();
        }

        void RegistryProxy::make_unregister_request(MemBuffer& request,
                                                    const std::string& topic)
        {
                request.printf("{\"request\": \"unregister\", \"topic\": \"%s\"}",
                               topic.c_str());
        }
                
        void RegistryProxy::send_request(MemBuffer& request)
        {
                bool success = websocket_->send(request, kTextMessage);
                if (!success) {
                        throw std::runtime_error("RegistryProxy::send_request failed");
                }
        }

        void RegistryProxy::response_assert_success()
        {
                MemBuffer response;
                read_response(response);
                nlohmann::json json = parse_response(response);
                assert_success(json);
        }

        void RegistryProxy::read_response(MemBuffer& response)
        {
                RecvStatus status = websocket_->recv(response, 2.0);
                if (status == kRecvTimeOut)
                        throw std::runtime_error("RegistryProxy::read_response: Time-out");
                else if (status == kRecvError)
                        throw std::runtime_error("RegistryProxy::read_response: Error");
        }
        
        nlohmann::json RegistryProxy::parse_response(MemBuffer& response)
        {
                nlohmann::json json;
                std::string text = response.tostring();
                try {
                        json = nlohmann::json::parse(text.c_str());
                } catch (nlohmann::json::exception& jerr) {
                        log_err(log_, "RegistryProxy: %s", jerr.what());
                        throw std::runtime_error("RegistryProxy::parse_response failed");
                }
                return json;
        }
        
        bool RegistryProxy::is_success(nlohmann::json& jsonobj)
        {
                bool retval = false;
                try {
                        retval = jsonobj["success"];
                } catch (nlohmann::json::exception& jerr) {
                        log_err(log_, "RegistryProxy: %s", jerr.what());
                }
                return retval;
        }
        
        void RegistryProxy::assert_success(nlohmann::json& jsonobj)
        {
                bool success = false;
                try {
                        success = jsonobj["success"];
                } catch (nlohmann::json::exception& jerr) {
                        log_err(log_, "RegistryProxy: %s", jerr.what());
                }
                if (!success) {
                        throw std::runtime_error("RegistryProxy::assert_success failed");
                }
        }
        
        void RegistryProxy::print_error(nlohmann::json& jsonobj, const std::string& method)
        {
                log_err(log_, "RegistryProxy: %s: Request failed", method.c_str());
                try {
                        log_err(log_, "RegistryProxy: Reason: %s",
                                  jsonobj["message"].dump().c_str());
                } catch (nlohmann::json::exception& jerr) {
                        log_err(log_, "RegistryProxy: %s", jerr.what());
                }
        }
                                
        bool RegistryProxy::read_address(IAddress& address)
        {
                bool found = false;
                MemBuffer response;
                read_response(response);
                
                nlohmann::json jsonobj = parse_response(response);
                assert_success(jsonobj);
                
                std::string address_string;
                found = get_address(jsonobj, address_string);
                if (found) {
                        address.set(address_string);
                }
                return found;
        }
        
        bool RegistryProxy::get_address(nlohmann::json& jsonobj,
                                        std::string& address_string)
        {
                bool found = false;
                try {
                        address_string = jsonobj["address"];
                        found = true;
                } catch (nlohmann::json::exception& jerr) {
                }
                return found;
        }
}

