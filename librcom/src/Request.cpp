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
#include "rcom/Log.h"
#include "rcom/Request.h"
#include "rcom/util.h"

namespace rcom {
                        
        Request::Request() : method_(kUnsupportedMethod), uri_(), headers_()
        {}

        IRequest::Method Request::get_method()
        {
                return method_;
        }
        
        void Request::set_method(IRequest::Method method)
        {
                method_ = method;
        }
        
        std::string& Request::get_uri()
        {
                return uri_;
        }
        
        void Request::set_uri(const std::string& uri)
        {
                uri_ = uri;
        }
        
        void Request::add_header(const std::string& name, const std::string& value)
        {
                headers_.push_back(RequestHeader(name, value));
        }
        
        bool Request::get_header_value(const std::string& name, std::string& value)
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
        
        void Request::assert_websocket()
        {
                http_method_is_get();
                has_valid_websocket_key();
                has_valid_websocket_version();
                connection_header_is_upgrade();
                upgrade_header_is_websocket();
        }

        void Request::http_method_is_get()
        {
                if (method_ != kGetMethod) {
                        throw std::runtime_error("Request: Bad HTTP method");
                }
        }

        void Request::has_valid_websocket_key()
        {
                std::string header_value;
                
                bool has = get_header_value("Sec-WebSocket-Key", header_value);
                if (!has) {
                        throw std::runtime_error("Request: Missing "
                                                 "Sec-WebSocket-Key header");
                }
                is_valid_websocket_key(header_value);
        }
        
        void Request::is_valid_websocket_key(const std::string& key)
        {
                if (key.length() != 24) {
                        throw std::runtime_error("Request: bad key length");
                } else if (key[22] != '=' || key[23] != '=') {
                        throw std::runtime_error("Request: expected closing '=' chars");
                } else if (!is_base64_string(key)) {
                        throw std::runtime_error("Request: invalid char");
                }
        }

        void Request::has_valid_websocket_version()
        {
                std::string header_value;
        
                bool valid = (get_header_value("Sec-WebSocket-Version", header_value)
                              && (header_value == "13"));
        
                if (!valid) {
                        throw std::runtime_error("Request: Bad "
                                                 "Sec-WebSocket-Version header");
                }
        }

        void Request::connection_header_is_upgrade()
        {
                std::string header_value;

                // Firefox sends "keep-alive, Upgrade" for the
                // Connexion header. So we simple check for the
                // presence of Upgrade in the header value.
                bool valid = (get_header_value("Connection", header_value)
                              && (header_value.find("Upgrade") != std::string::npos
                                  || header_value.find("upgrade") != std::string::npos));
        
                if (!valid) {
                        throw std::runtime_error("Request: Bad Connection header");
                }
        }

        void Request::upgrade_header_is_websocket()
        {
                std::string header_value;
                
                bool valid = (get_header_value("Upgrade", header_value)
                              && (header_value == "websocket"));
                
                if (!valid) {
                        throw std::runtime_error("Request: Bad Upgrade header");
                }
        }
}

