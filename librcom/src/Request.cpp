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
#include <r.h>
#include "Request.h"
#include "util.h"

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
        
        bool Request::is_websocket()
        {
                return http_method_is_get()
                        && has_valid_websocket_key()
                        && has_valid_websocket_version()
                        && connection_header_is_upgrade()
                        && upgrade_header_is_websocket();
        }

        bool Request::http_method_is_get()
        {
                return method_ == kGetMethod;
        }

        bool Request::has_valid_websocket_key()
        {
                bool valid;
                std::string header_value;
        
                valid = (get_header_value("Sec-WebSocket-Key", header_value)
                         && is_valid_websocket_key(header_value));
        
                if (!valid) {
                        r_warn("Request: Bad Sec-WebSocket-Key header");
                }

                return valid;
        }
        
        bool Request::is_valid_websocket_key(const std::string& key)
        {
                bool valid = false;
                if (key.length() != 24) {
                        r_debug("is_valid_websocket_key: bad key length: '%s'", key.c_str());
                } else if (key[22] != '=' || key[23] != '=') {
                        r_debug("is_valid_websocket_key: expected closing '=' chars");
                } else if (!is_base64_string(key)) {
                        r_debug("is_valid_websocket_key: invalid char");
                } else {
                        valid = true;
                }
                return valid;
        }

        bool Request::has_valid_websocket_version()
        {
                std::string header_value;
        
                bool valid = (get_header_value("Sec-WebSocket-Version", header_value)
                              && (header_value.compare("13") == 0));
        
                if (!valid) {
                        r_warn("Request: Bad Sec-WebSocket-Version header");
                }

                return valid;
        }

        bool Request::connection_header_is_upgrade()
        {
                std::string header_value;

                // Firefox sends "keep-alive, Upgrade" for the
                // Connexion header. So we simple check for the
                // presence of Upgrade in the header value.
                bool valid = (get_header_value("Connection", header_value)
                              && (header_value.find("Upgrade") != std::string::npos
                                  || header_value.find("upgrade") != std::string::npos));
        
                if (!valid) {
                        r_warn("Request: Bad Connection header");
                }

                return valid;
        }

        bool Request::upgrade_header_is_websocket()
        {
                std::string header_value;
                
                bool valid = (get_header_value("Upgrade", header_value)
                              && ((header_value.compare("websocket") == 0)
                                  || (header_value.compare("WebSocket") == 0)));
                
                if (!valid) {
                        r_warn("Request: Bad Upgrade header: %s", header_value.c_str());
                }

                return valid;
        }
}

