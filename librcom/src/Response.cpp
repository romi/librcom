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
#include "rcom/Response.h"

namespace rcom {

        Response::Response()
                : code_(0), headers_()
        {}

        int Response::get_code()
        {
                return code_;
        }
        
        void Response::set_code(int code)
        {
                code_ = code;
        }
        
        void Response::add_header(const std::string& name, const std::string& value)
        {
                headers_.push_back(ResponseHeader(name, value));
        }
        
        bool Response::get_header_value(const std::string& name, std::string& value)
        {
                bool found = false;
                value = "";
                for (size_t i = 0; i < headers_.size(); i++) {
                        if (name.compare(headers_[i].name_) == 0) {
                                value = headers_[i].value_;
                                found = true;
                                break;
                        }
                }
                return found;
        }
                
        void Response::assert_websocket(const std::string& accept)
        {
                status_equals_101();
                connection_header_equals_upgrade();
                upgrade_header_equals_websocket();
                accept_header_is_valid(accept);
        }

        void Response::status_equals_101()
        {
                if (code_ != 101) {
                        throw std::runtime_error("Response: Expected 101 status");
                }
        }

        bool Response::header_equals(const std::string& name,
                                     const std::string& expected)
        {
                std::string value;
                bool found = get_header_value(name, value);
                return (found && value.compare(expected) == 0);
        }
        
        void Response::connection_header_equals_upgrade()
        {
                bool valid = (header_equals("Connection", "Upgrade")
                              || header_equals("Connection", "upgrade"));                  
                if (!valid) {
                        throw std::runtime_error("Response: Bad 'Connection' header");
                }
        }
        
        void Response::upgrade_header_equals_websocket()
        {
                bool valid = header_equals("Upgrade", "websocket");
                if (!valid) {
                        throw std::runtime_error("Response: Bad 'Upgrade' header");
                }
        }

        void Response::accept_header_is_valid(const std::string& accept)
        {
                bool valid = header_equals("Sec-WebSocket-Accept", accept);
                if (!valid) {
                        throw std::runtime_error("Response: Bad 'Sec-WebSocket-Accept' "
                                                 "header");
                }
        }
}
