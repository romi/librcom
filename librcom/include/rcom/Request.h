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
#ifndef _LIBRCOM_REQUEST_H_
#define _LIBRCOM_REQUEST_H_

#include <vector>
#include "rcom/IRequest.h"

namespace rcom {

        struct RequestHeader
        {
                std::string name_;
                std::string value_;

                RequestHeader(const std::string& name, const std::string& value)
                        : name_(name), value_(value)
                        {}
        };

        class Request : public IRequest
        {
        private:
                Method method_;
                std::string uri_;
                std::vector<RequestHeader> headers_;

                bool http_method_is_get();
                bool has_valid_websocket_key();
                bool is_valid_websocket_key(const std::string& key);
                bool has_valid_websocket_version();
                bool connection_header_is_upgrade();
                bool upgrade_header_is_websocket();
                
        public:
                        
                Request(); 
                virtual ~Request() override = default; 

                Method get_method() override;
                void set_method(Method method) override;
                
                std::string& get_uri() override;
                void set_uri(const std::string& uri) override;

                void add_header(const std::string& name, const std::string& value) override;
                bool get_header_value(const std::string& name, std::string& value) override;
                
                bool is_websocket() override;
        };
}

#endif // _LIBRCOM_REQUEST_H_

