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
#ifndef _LIBRCOM_REGISTRY_PROXY_H_
#define _LIBRCOM_REGISTRY_PROXY_H_

#include <memory>
#include <MemBuffer.h>
#include <JsonCpp.h>
#include "IRegistry.h"
#include "ISocketFactory.h"
#include "IClock.h"

namespace rcom {

        class RegistryProxy : public IRegistry
        {
        protected:
                std::unique_ptr<IWebSocket> websocket_;
                IClock& clock_;
                        
                void make_register_request(rpp::MemBuffer& request,
                                           const std::string& topic,
                                           IAddress& address);
                void make_unregister_request(rpp::MemBuffer& request,
                                             const std::string& topic);
                
                void make_get_request(rpp::MemBuffer& request, const std::string& topic);
                
                bool send_request(rpp::MemBuffer& request); 
                bool response_is_success(const std::string& method); 
                bool read_response(rpp::MemBuffer& response); 
                JsonCpp parse_response(rpp::MemBuffer& response); 
                bool is_success(JsonCpp& json); 
                void print_error(JsonCpp& json, const std::string& method); 
                bool read_address(IAddress& address); 
                bool get_address(JsonCpp& json, std::string& address_string);

        public:
                RegistryProxy(std::unique_ptr<IWebSocket>& websocket, IClock& clock);
                virtual ~RegistryProxy() override;

                bool set(const std::string& topic, IAddress& address) override; 
                bool get(const std::string& topic, IAddress& address,
                         double timeout_in_seconds = 12.0) override;
                bool remove(const std::string& topic) override; 
        };
}

#endif // _LIBRCOM_REGISTRY_PROXY_H_

